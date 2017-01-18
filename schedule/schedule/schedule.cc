#include "schedule/schedule/schedule.h"

void Schedule::merge_batch_schedule_in(std::unique_ptr<BatchSchedule> bs) {
  lock_table.merge_into_lock_table(bs->lock_table);
}

std::shared_ptr<Txn> Schedule::get_txn_to_execute() {
  return lock_table.get_next_ready_txn();
}

std::shared_ptr<Txn> Schedule::try_get_txn_to_execute() {
  return lock_table.try_get_next_ready_txn();
}

void Schedule::finalize_txn(std::shared_ptr<Txn> t) {
  auto wh = t->get_write_set_handle();
  auto rh = t->get_read_set_handle();

  for (auto it = rh->rbegin(); it != rh->rend(); ++it) {
    lock_table.finalize_lock_request(*it);
  }

  for (auto it = wh->rbegin(); it != wh->rend(); ++it) {
    lock_table.finalize_lock_request(*it);
  }
}
