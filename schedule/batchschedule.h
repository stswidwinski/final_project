#ifndef _BATCH_SCHEDULE_H_
#define _BATCH_SCHEDULE_H_

#include "schedule/locktable.h"
#include "txns/txn.h"

#include <unordered_set>
#include <memory>
#include <vector>

class BatchSchedule {
private:
  LockTable lock_table;

public:
  void add_txn(Txn* t);
  LockTable& get_lock_table();
  // Can be thought of as static constructor
  static std::unique_ptr<BatchSchedule> build_batch_schedule(
      std::unique_ptr<std::vector<Txn>> batch);

  bool operator==(const BatchSchedule& bs) const;
};

#endif // _BATCH_SCHEDULE_H_
