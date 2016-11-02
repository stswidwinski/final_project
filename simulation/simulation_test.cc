#include "simulation/txn_map.h"
#include "simulation/txn_generator.h"
#include "txns/txn.h"
#include "schedule/schedule.h"
#include "schedule/batchschedule.h"
#include "simulation/utils.h"
#include "simulation/sim_args_parser.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <unordered_map>

SimulationArgs args;

void run_simulation_for_model(TxnMap& load, Model model);

int main(int argc, char** argv) {
  args = parse_arguments(argc, argv); 

  write_descr_header(args.dump_path_root);
  TxnGenerator txn_gen(args.txn_gen_params);
  // run 10 of each! we will average out afterwards.
  for (unsigned int i = 0; i < args.reps; i ++) {
    std::cerr << "Uniform trial " << i + 1 << " Out of " << args.reps << ": prepping load\r";
    auto uniform_load = TxnMap(txn_gen.gen_uniform()); 
    
    if (args.requested_data.find(Data::load) != args.requested_data.end())
      write_txn_load(args.dump_path_root, "uniform", args.txn_gen_params, uniform_load);

    for (auto& model : args.requested_models) {
      std::cerr << "Uniform trial " << i + 1 << " Out of " << args.reps <<": running model " << (int) model + 1 << " out of " << args.requested_models.size() << "\r";
      run_simulation_for_model(uniform_load, model);
    }    
  }

  std::cerr << std::endl;
  // run experiments for the bursty distribution
  for (unsigned int i = 0; i < args.reps; i++) {
    std::cerr << "Bursty trial " << i + 1 << " out of " << args.reps << ": prepping load\r";
    auto bursty_load = TxnMap(txn_gen.gen_bursty());
    
    if (args.requested_data.find(Data::load) != args.requested_data.end())
      write_txn_load(args.dump_path_root, "bursty", args.txn_gen_params, bursty_load);
    
    for (auto& model : args.requested_models) {
      std::cerr << "Bursty trial " << i + 1 << " Out of " << args.reps << ": running model " << (int) model + 1 << " out of " << args.requested_models.size() << "\r";
      run_simulation_for_model(bursty_load, model);
    } 
  }
  std::cerr << std::endl;

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
  Schedule sched;

  if (model == Model::sequential)
    threads = 1;

  if (model != Model::batched)
    batch_length = 1;
  
  std::vector<unsigned int> in_flight_txn_in_time;

  while (current_time <= args.time_period ||
      in_flight_txns.size() != 0) {

    in_flight_txn_in_time.push_back(in_flight_txns.size());
    // first, get all the finished txns and finalize them 
    auto finished_txns_range = in_flight_txns.equal_range(current_time);
    for(auto curr_it = finished_txns_range.first; 
        curr_it != finished_txns_range.second; 
        curr_it++) {
      sched.finalize_txn((*curr_it).second);
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
    write_avg_std_dev(
        args.dump_path_root, 
        model, 
        args.txn_gen_params, 
        load, 
        batch_length,
        current_time);
  }

  if (args.requested_data.find(Data::locks_in_time) != args.requested_data.end()) {
    write_locks_in_time(
      args.dump_path_root,
      model,
      args.txn_gen_params,
      in_flight_txn_in_time,
      batch_length);
  }
}
