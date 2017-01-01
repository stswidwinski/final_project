#ifndef _SIM_DATA_H_
#define _SIM_DATA_H_

#include <set>

enum class Data {
  load,
  avg_proc_time,
  std_dev_proc_time,
  locks_in_time,
  dep_graph,
  txn_gant,
  data_types_count
};

std::set<Data> get_req_data(int argc, char** argv);

#endif // _SIM_DATA_H
