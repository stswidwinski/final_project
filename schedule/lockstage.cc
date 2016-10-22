#include "schedule/lockstage.h"

bool LockStage::operator==(const LockStage& ls) const {
  return (
      holders == ls.holders &&
      type == ls.type &&
      requesters == ls.requesters);
}

bool LockStage::operator!=(const LockStage& ls) const {
  return !(LockStage::operator==(ls));
}

bool LockStage::add_to_stage(Txn* txn, LockType req_type) {
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

int LockStage::get_current_holders() {
  return holders;
}

std::shared_ptr<LockStage> LockStage::get_next_request() {
  return next_request;
}

std::unordered_set<Txn*>& LockStage::get_requesters() {
  return requesters;
}

LockType LockStage::get_lock_type() {
  return type;
}
