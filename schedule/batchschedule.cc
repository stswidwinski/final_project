#include "containers/array_cont.h"
#include "schedule/batchschedule.h"
#include "packing/packing.h"

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

std::unique_ptr<BatchSchedule> BatchSchedule::build_batch_schedule(
    std::unique_ptr<std::vector<Txn>> b) {
  std::unique_ptr<BatchSchedule> bs = std::make_unique<BatchSchedule>();
  ArrayContainer c(std::move(b));
  while(c.get_remaining_count() != 0) {
    c.sort_remaining();
    auto packing = get_packing(&c);
    for (Txn& t : packing) {
      bs->add_txn(&t);
    }
  }

  return bs; 
}
