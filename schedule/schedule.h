#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "schedule/locktable.h"
#include "schedule/batchschedule.h"

class Schedule {
private:
  LockTable lock_table;
public:
  void merge_batch_schedule_in(BatchSchedule &bs);
  // blocks until a ready txn is available.
  Txn* get_txn_to_execute();
  void finalize_txn(Txn* t);
};

#endif // _SCHEDULE_H_
