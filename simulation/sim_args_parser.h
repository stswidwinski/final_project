#ifndef _SIM_ARGS_PARSER_H_
#define _SIM_ARGS_PARSER_H_

#include "simulation/txn_generator.h"

#include <set>

// the following are the legal models
enum class Model {
  sequential,
  real_time,
  batched,
  count
};

// the following are the legal data dump requests
enum class Data {
  load,
  avg_proc_time,
  std_dev_proc_time,
  locks_in_time,
  dep_graph,
  txn_gant
};

struct SimulationArgs {
  TxnGeneratorParams txn_gen_params;
  unsigned int batch_length;
  unsigned int time_period;
  unsigned int reps;
  std::string dump_path_root;
  std::set<Model> requested_models;
  std::set<Data> requested_data;
};

/*
 *  The arguments are given in the following order:
 *    1) The workload creation parameters -- details within txn_generator.h
 *       a) lock_time_multiplier (unsigned int)   1)
 *       b) time_period          (unsigned int)   2)
 *       c) transaction number   (unsigned int)   3)
 *       d) uncontested locks
 *          i) Lock Space Size   (unsigned int)   4)
 *          ii) Avg # held       (unsigned int)   5)
 *          iii) Held # std dev  (unsigned int)   6)
 *       e) contested locks
 *          i) Lock Space Size   (unsigned int)   7)
 *          ii) Avg # held       (unsigned int)   8)
 *          iii) Held # std dev  (unsigned int)   9)
 *       f) % of write txns      (double)         10)
 *       g) bursty seed chance   (double)         11)
 *       h) bursty linear factor (unsigned int)   12)
 *    2) Batch Length (unsigned int)              13)
 *    3) Output file dir                          14)
 *    4) Repetitions of experiments               15)
 *  The following two are given as strings after the former. Any number of either
 *  can be given, but at least one of each must be present.
 *    5) The models requested                     16 - *)
 *    6) The data dumps requested                 *) - *)
 */
SimulationArgs parse_arguments(int argc, char** argv);

#endif //_SIM_ARGS_PARSER_H_
