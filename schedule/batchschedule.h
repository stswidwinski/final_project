#ifndef _BATCH_SCHEDULE_H_
#define _BATCH_SCHEDULE_H_

#include "schedule/locktable.h"
#include "txns/txn.h"

class BatchSchedule {
private:
  LockTable lock_table;

public:
  void add_txn(Txn* t);
  LockTable& get_lock_table();
};

#endif // _BATCH_SCHEDULE_H_
