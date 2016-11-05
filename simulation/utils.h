#include "simulation/txn_sim_wrapper.h"
#include "simulation/txn_map.h"
#include "utils/debug.h"
#include "simulation/txn_generator.h"
#include "simulation/sim_args_parser.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

std::string model_to_string(Model model) {
  switch(model) {
    case Model::sequential:
      return "sequential";
    case Model::real_time:
      return "real_time";
    case Model::batched:
      return "batched";
    default:
      return "UNKNOWN";
  }
}

std::string append_path(std::string base, std::string path) {
  if (base.back() != '/')
    base += '/';

  return base + path;
}

void write_txn_load(
    std::string dump_path,
    std::string load_type,
    TxnMap txn_load) {
  static int load_num = 0;
  std::ofstream dump_file;
  std::string path = append_path(
      dump_path, 
      load_type + "_load_" + std::to_string(load_num));
  load_num ++;

  // write the actual result
  try {
    dump_file.open(path, std::ios::out | std::ios::trunc);
  } catch (const std::ofstream::failure& e) {
    DEBUG_CERR(std::cerr << e.what() << ". ERR CODE:" << e.code() << std::endl;);
  }

  dump_file << "arrival_time,duration\n";
  for (const TxnWrapper& tw : txn_load.txns) {
    dump_file << tw.arrival_time << "," << tw.exec_duration << "\n";
  }

  dump_file.close();
}

void write_avg_std_dev(
    std::string dump_path, 
    Model model, 
    TxnMap load,
    unsigned int batch_length,
    unsigned int total_time) {
  std::ofstream dump_file;
  std::string path = append_path(dump_path, model_to_string(model) + "_avg_std");
  try {
    dump_file.open(path, std::ios::out | std::ios::app);
  } catch (const std::ofstream::failure& e) {
    DEBUG_CERR(std::cerr << e.what() << ". ERR CODE:" << e.code() << std::endl;);
  }
  
  if (dump_file.tellp() == 0) {
    // write the header
    dump_file << "average_completion_time,standard_deviation,batch_length,total_time\n";
  }

  dump_file << load.get_average_completion_time() << "," 
            << load.get_std_deviation_completion_time() << "," 
            << batch_length << ","
            << total_time << "\n";

  dump_file.close();
}

void write_txns_in_time(
    std::string dump_path, 
    Model model, 
    std::vector<unsigned int> locks_in_time,
    unsigned int batch_length) {
  static unsigned int counters[(int) Model::count] = {0};
  std::ofstream dump_file;
  
  std::string path = append_path(
      dump_path, 
      model_to_string(model) + "_txns_in_time_" + std::to_string(counters[(int) model]));
  counters[(int) model] ++;

  try {
    dump_file.open(path, std::ios::out | std::ios::app);
  } catch (const std::ofstream::failure& e) {
    DEBUG_CERR(std::cerr << e.what() << ". ERR CODE:" << e.code() << std::endl;);
  }
  
  if (dump_file.tellp() == 0) {
    // write the header
    dump_file << "number_of_txns,time\n";
  }

  for (unsigned int i = 0; i < locks_in_time.size(); i++)
    dump_file << locks_in_time[i] << "," << i << "\n";

  dump_file.close();
}
