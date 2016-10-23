#ifndef _BATCH_SCHEDULE_H_
#define _BATCH_SCHEDULE_H_

#include "schedule/locktable.h"
#include "txns/txn.h"
#include "containers/container.cc"
#include "utils/debug.h"

#include <unordered_set>
#include <memory>
#include <vector>

class BatchSchedule {
private:
  LockTable lock_table;
  void add_txn(std::shared_ptr<Txn> t);
  // NOTE: used for debugging and testing only.
  DEBUG_VARIABLE(std::unordered_set<int, std::shared<Txn>>, txns); 

public:
  // Can be thought of as static constructor
  static std::unique_ptr<BatchSchedule> build_batch_schedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch);

  bool operator==(const BatchSchedule& bs) const;

  // tests that need access
  friend void BatchScheduleInsert();
  friend void ScheduleMergingIntoEmptyTest();
  friend void ScheduleMergingIntoExistingTest();

  friend class Schedule;  
};

#endif // _BATCH_SCHEDULE_H_
