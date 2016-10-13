#include "schedule/schedule.h"

void Schedule::merge_batch_schedule_in(BatchSchedule& bs) {
  lock_table.merge_into_lock_table(bs.get_lock_table());
}

Txn* Schedule::get_txn_to_execute() {
  return lock_table.get_next_ready_txn();
}

void Schedule::finalize_txn(Txn* t) {
  auto wh = t->get_write_set_handle();
  auto rh = t->get_read_set_handle();

  for (auto it = rh->rbegin(); it != rh->rend(); ++it) {
    lock_table.finalize_lock_request(*it);
  }

  for (auto it = wh->rbegin(); it != wh->rend(); ++it) {
    lock_table.finalize_lock_request(*it);
  }
}
