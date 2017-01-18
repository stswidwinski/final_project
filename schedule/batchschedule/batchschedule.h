#ifndef _BATCH_SCHEDULE_H_
#define _BATCH_SCHEDULE_H_

#include "schedule/locktable/locktable.h"
#include "txns/txn.h"
#include "containers/container.cc"
#include "utils/debug.h"

#include <unordered_map>
#include <memory>
#include <vector>

class BatchSchedule {
protected:
  LockTable lock_table;
  void add_txn(std::shared_ptr<Txn> t);

#ifdef DEBUG
  // NOTE: used for debugging and testing only.
  std::unordered_map<unsigned int, std::shared_ptr<Txn>> txns;
#endif

public:
  BatchSchedule(){};
  BatchSchedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> b);

  // Can be thought of as static constructor
  static std::unique_ptr<BatchSchedule> build_batch_schedule(
      std::unique_ptr<std::vector<std::unique_ptr<Txn>>> batch);

  friend bool operator==(
      const BatchSchedule& bs1, 
      const BatchSchedule& bs2);

  friend class TestBatchSchedule;
  friend class Schedule; 
  friend class ScheduleSnooper;
};

#endif // _BATCH_SCHEDULE_H_
