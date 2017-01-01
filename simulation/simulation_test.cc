#include "simulation/txn_map.h"
#include "simulation/txn_generator.h"
#include "txns/txn.h"
#include "schedule/schedule.h"
#include "schedule/batchschedule.h"
#include "simulation/schedule_snooper.h"
#include "simulation/utils.h"
#include "simulation/sim_args_parser.h"
#include "simulation/model.h"
#include "simulation/workload.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <unordered_map>

SimulationArgs args;

void run_simulation_for_model(TxnMap& load, Model model);

int main(int argc, char** argv) {
  args = parse_arguments(argc, argv); 
  TxnGenerator txn_gen(args.txn_gen_params);
  auto run = [&txn_gen](Workload load){
    for (unsigned int i = 0; i < args.reps; i ++) {
      begin_print_section();
  
      auto trial_string = "Trial " + std::to_string(i+1) +\
                          " Out Of " + std::to_string(args.reps);
      put(trial_string + "\n");
  
      put("Prepping load");
      auto work_load = load == Workload::uniform ? 
        TxnMap(txn_gen.gen_uniform()) :
        TxnMap(txn_gen.gen_bursty()); 
      put(" [ OK ]\n", "green");

      if (args.requested_data.find(Data::load) != args.requested_data.end()) {
        put("Dumping workload data ");
        write_txn_load(args.dump_path_root, workload_to_string(load), work_load);
        put(" [ OK ]\n", "green");
      }

      for (auto& model : args.requested_models) {
        auto model_string = "Running " + model_to_string(model) + " model simulation";
        begin_print_section();
        put(model_string);
        run_simulation_for_model(work_load, model);
        wipe_section();
        put(model_string);
        put(" [ OK ]\n", "green");
      }

      wipe_section();
      put(trial_string);
      put(" [ OK ]\n", "green");
    }
  };

  begin_print_section(); 
  put("Beginning experiments!\n");

  begin_print_section();
  put("Uniform Workload Trials\n");
  run (Workload::uniform);
  wipe_section();
  put("Uniform Workload Trials");
  put(" [ OK ]\n", "green");
  
  begin_print_section();
  put("Bursty Workload Trials\n");
  run (Workload::bursty);
  wipe_section();

  wipe_section();
  return 0;
}

void run_simulation_for_model(TxnMap& load, Model model) { 
  // TODO: allow limited threads.
  int threads = -1;
  typedef unsigned int CompletionTime;
  typedef std::unordered_multimap<CompletionTime, std::shared_ptr<Txn>> 
    InflightTxnMap;
  
  load.reset_start_time();
  // indexed on end time.
  InflightTxnMap in_flight_txns;
  unsigned int current_time = 0;
  unsigned int batch_length = args.batch_length;
  unsigned int txns_processed = 0;
  Schedule sched;
  ScheduleSnooper snoop;

  if (model == Model::sequential)
    threads = 1;

  if (model != Model::batched)
    batch_length = 1;
  
  std::vector<unsigned int> in_flight_txn_in_time;

  while (
      txns_processed < args.txn_gen_params.txn_number &&
      (current_time <= args.time_period ||
      in_flight_txns.size() != 0)) {

    in_flight_txn_in_time.push_back(in_flight_txns.size());
    // first, get all the finished txns and finalize them 
    auto finished_txns_range = in_flight_txns.equal_range(current_time);
    for(auto curr_it = finished_txns_range.first; 
        curr_it != finished_txns_range.second; 
        curr_it++) {
      sched.finalize_txn((*curr_it).second);
      txns_processed ++;
    }
    in_flight_txns.erase(current_time);

    // check if its time for a new batch. If so, make it and
    // incorporate it with the schedule
    if (current_time != 0 &&
        (current_time % batch_length) == 0) {
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch;
      batch = std::make_unique<std::vector<std::unique_ptr<Txn>>>(
          std::move(load.get_cpy_of_txns_between(current_time - batch_length, current_time - 1)));
      auto bs = BatchSchedule::build_batch_schedule(std::move(batch));
      if (args.requested_data.find(Data::dep_graph) != args.requested_data.end()) {
        snoop.update_snooper(bs.get());
      } 
      sched.merge_batch_schedule_in(std::move(bs));
   }

    // get all the txns that are ready, start "execution"
    std::shared_ptr<Txn> t;
    // only allow up to threads txns in flight unless we have unlimited threads (<0)
    while((threads < 0 || in_flight_txns.size() < (unsigned int) threads) &&
        (t = sched.try_get_txn_to_execute()) != nullptr) {
      load.set_start_time(t->get_id(), current_time);
      in_flight_txns.insert(
          {load.get_completion_time(t->get_id()), t});  
    }

    current_time++;
  }

  // simulation done. Dump the results
  if (args.requested_data.find(Data::avg_proc_time) != args.requested_data.end() || 
      args.requested_data.find(Data::std_dev_proc_time) != args.requested_data.end()) {
    put("Dump avg proc time and std dev proc time");
    write_avg_std_dev(
        args.dump_path_root, 
        model, 
        load, 
        batch_length,
        current_time);
    put(" [ OK ]\n", "green");
  }

  if (args.requested_data.find(Data::locks_in_time) != args.requested_data.end()) {
    put("Dump txns in time");
    write_txns_in_time(
      args.dump_path_root,
      model,
      in_flight_txn_in_time,
      batch_length);
    put(" [ OK ]\n", "green");
  }

  if (args.requested_data.find(Data::dep_graph) != args.requested_data.end()) {
    put("Dump dependency graph");
    snoop.print_dependencies(args.dump_path_root, model);
    put(" [OK]\n", "green");
  }

  if (args.requested_data.find(Data::txn_gant) != args.requested_data.end()) {
    put("Dump gant txn info");
    write_txn_gant(args.dump_path_root, model, load); 
    put(" [OK]\n", "green");
  }
}
