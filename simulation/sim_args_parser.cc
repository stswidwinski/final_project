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

std::set<Model> get_models(int argc, char** argv) {
  std::set<Model> res;
  for (int i = 15; i < argc; i++) {
    if (strcmp(argv[i], "sequential") == 0) {
      res.insert(Model::sequential);
    } else if (strcmp(argv[i], "real_time") == 0) {
      res.insert(Model::real_time);
    } else if (strcmp(argv[i], "batched") == 0) {
      res.insert(Model::batched);
    }
  }

  if (res.size() == 0) {
    std::cerr << "ERROR. No models specified\n";
    exit(1);
  }

  return res;
}

std::set<Data> get_req_data(int argc, char** argv) {
  std::set<Data> res;
  for (int i = 15; i < argc; i++) {
    if (strcmp(argv[i], "load") == 0) {
      res.insert(Data::load);
    } else if (strcmp(argv[i], "avg_proc_time") == 0) {
      res.insert(Data::avg_proc_time);
    } else if (strcmp(argv[i], "std_dev_proc_time") == 0) {
      res.insert(Data::std_dev_proc_time);
    } else if (strcmp(argv[i], "locks_in_time") == 0) {
      res.insert(Data::locks_in_time);
    } else if (strcmp(argv[i], "dep_graph") == 0) {
      res.insert(Data::dep_graph);
    } else if (strcmp(argv[i], "txn_gant") == 0) {
      res.insert(Data::txn_gant);
    }
  }

  if (res.size() == 0) {
    std::cerr << "ERROR. No data requested\n";
    exit(1);
  }

  return res;
}

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
