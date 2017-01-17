#include "schedule/lockstage.h"

bool LockStage::add_to_stage(std::shared_ptr<Txn> txn, LockType req_type) {
  if ((req_type == LockType::exclusive && requesters.size() != 0) ||
      type == LockType::exclusive) {
    return false;
  }

  requesters.insert(txn);
  type = req_type;
  holders ++;
  return true;
}

int LockStage::decrement_holders() {
  return (--holders);
}

void LockStage::set_next_stage(std::shared_ptr<LockStage> ls) {
  next_request = ls;
}

int LockStage::get_current_holders() const {
  return holders;
}

std::shared_ptr<LockStage> LockStage::get_next_request() {
  return next_request;
}

const std::unordered_set<std::shared_ptr<Txn>>& LockStage::get_requesters() const {
  return requesters;
}

LockType LockStage::get_lock_type() const {
  return type;
}
