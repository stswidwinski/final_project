#include "containers/array_cont.h"
#include "schedule/batchschedule.h"
#include "packing/packing.h"

bool BatchSchedule::operator==(const BatchSchedule& bs) const {
  return bs.lock_table == lock_table;
};

void BatchSchedule::add_txn(std::shared_ptr<Txn> t) {
  auto wh = t->get_write_set_handle();
  auto rh = t->get_read_set_handle();

  for (auto& lck : (*wh)) {
    lock_table.insert_lock_request(t, lck, LockType::exclusive);
  }

  for (auto& lck : (*rh)) {
    lock_table.insert_lock_request(t, lck, LockType::shared);
  }
}

std::unique_ptr<BatchSchedule> BatchSchedule::build_batch_schedule(
    std::unique_ptr<std::vector<std::unique_ptr<Txn>>> b) {
  std::unique_ptr<BatchSchedule> bs = std::make_unique<BatchSchedule>();
  ArrayContainer c(std::move(b));
  while(c.get_remaining_count() != 0) {
    c.sort_remaining();
    auto packing = get_packing(&c);
    for (unsigned int i = 0; i < packing.size(); i++) {
      // the ownership of t will now be spread out across multiple
      // lock table requests.
      std::shared_ptr<Txn> t{std::move(packing[i])};
#ifdef DEBUG
      bs->txns.insert({t->get_id(), t});
#endif
      bs->add_txn(t);
    }
  }

  return bs; 
}
