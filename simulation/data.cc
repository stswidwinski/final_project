#include "simulation/data.h"
#include <string.h>
#include <iostream>

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


