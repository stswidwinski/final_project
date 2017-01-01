#include "simulation/workload.h"

std::string workload_to_string(Workload load) {
  switch(load) {
    case Workload::uniform:
      return "uniform";
    case Workload::bursty:
      return "bursty";
    default:
      return "error";
  }
}
