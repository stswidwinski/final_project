#ifndef _SCHEDULE_SNOOPER_H_
#define _SCHEDULE_SNOOPER_H_

#include "schedule/schedule.h"
#include "schedule/lockstage.h"
#include "simulation/sim_args_parser.h"

#include <unordered_map>
#include <memory>
#include <vector>

class ScheduleSnooper {
private:
  typedef int lock_id;
  typedef unsigned int txn_id;
  std::unordered_map<lock_id, std::vector<std::shared_ptr<LockStage>>> lock_stages;
public:
  void update_snooper(BatchSchedule* bs);
  void print_dependencies(std::string dump_path, std::string model_name);
};

#endif //_SCHEDULE_SNOOPER_H_
