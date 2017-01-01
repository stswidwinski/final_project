#include "simulation/sim_args_parser.h"

#include <sstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>

template <typename T>
T parse_param(char*& string_param) {
  std::stringstream conv;
  T converted;
  conv << string_param;
  conv >> converted;
  return converted;
};

SimulationArgs parse_arguments(int argc, char** argv) {
  if (argc < 17) {
    std::cerr << "ERROR. Too few arguments for the generator\n";
    exit(1);
  }

  SimulationArgs sa;
  sa.time_period = parse_param<unsigned int>(argv[2]);
  sa.txn_gen_params = {
    .lock_time_multiplier = parse_param<unsigned int>(argv[1]),
    .txn_number = parse_param<unsigned int>(argv[3]),
    .time_period = sa.time_period,
    .cont_lock = {
      .lock_space = parse_param<unsigned int>(argv[7]),
      parse_param<unsigned int>(argv[8]),
      parse_param<unsigned int>(argv[9])
    },
    .uncont_lock = {
      .lock_space = parse_param<unsigned int>(argv[4]),
      parse_param<unsigned int>(argv[5]),
      parse_param<unsigned int>(argv[6])
    },
    .write_txns_perc = parse_param<double>(argv[10]),
    .seed_chance = parse_param<double>(argv[11]),
    .linear_factor = parse_param<unsigned int>(argv[12]) 
  };
  sa.batch_length = parse_param<unsigned int>(argv[13]);
  sa.dump_path_root = argv[14];
  sa.reps = parse_param<unsigned int>(argv[15]);
  sa.requested_models = get_models(argc, argv);
  sa.requested_data = get_req_data(argc, argv);

  return sa;
}
