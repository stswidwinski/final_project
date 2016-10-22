#include "schedule/batchschedule.h"

bool BatchSchedule::operator==(const BatchSchedule& bs) const {
  return bs.lock_table == lock_table;
};

void BatchSchedule::add_txn(Txn* t) {
  auto wh = t->get_write_set_handle();
  auto rh = t->get_read_set_handle();

  for (auto& lck : (*wh)) {
    lock_table.insert_lock_request(t, lck, LockType::exclusive);
  }

  for (auto& lck : (*rh)) {
    lock_table.insert_lock_request(t, lck, LockType::shared);
  }
}

LockTable& BatchSchedule::get_lock_table() {
  return lock_table;
}
