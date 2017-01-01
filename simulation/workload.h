#ifndef _SIM_WORKLOAD_H_
#define _SIM_WORKLOAD_H_

#include <string>

enum class Workload { 
  uniform, 
  bursty,
  workload_count
};

std::string workload_to_string(Workload load);

#endif // _SIM_WORKLOAD_H_
