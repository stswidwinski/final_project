#include "simulation/model.h"
#include <cstring>
#include <iostream>

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


