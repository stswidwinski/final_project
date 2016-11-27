#ifndef _BATCH_SCHEDULE_H_
#define _BATCH_SCHEDULE_H_

#include "schedule/locktable.h"
#include "txns/txn.h"
#include "containers/container.cc"
#include "utils/debug.h"

#include <unordered_map>
#include <memory>
#include <vector>

class BatchSchedule {
private:
  LockTable lock_table;
  void add_txn(std::shared_ptr<Txn> t);

#ifdef DEBUG
  // NOTE: used for debugging and testing only.
  std::unordered_map<unsigned int, std::shared_ptr<Txn>> txns;
#endif

public:
  // Can be thought of as static constructor
  static std::unique_ptr<BatchSchedule> build_batch_schedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch);

  bool operator==(const BatchSchedule& bs) const;

  // tests that need access
  friend void BatchScheduleInsert();
  friend void ScheduleMergingIntoEmptyTest();
  friend void ScheduleMergingIntoExistingTest();
  friend void BatchScheduleCreationTest1();
  friend void BatchScheduleCreationTest2();

  friend class Schedule;  
  friend class ScheduleSnooper;
};

#endif // _BATCH_SCHEDULE_H_
