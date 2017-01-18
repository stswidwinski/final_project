#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "schedule/locktable.h"
#include "schedule/batchschedule.h"

#include <memory>

class Schedule {
protected:
  LockTable lock_table;
public:
  void merge_batch_schedule_in(std::unique_ptr<BatchSchedule> bs);
  // blocks until a ready txn is available.
  std::shared_ptr<Txn> get_txn_to_execute();
  std::shared_ptr<Txn> try_get_txn_to_execute();
  void finalize_txn(std::shared_ptr<Txn> t);
};

#endif // _SCHEDULE_H_
