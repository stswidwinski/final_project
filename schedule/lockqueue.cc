#include "utils/debug.h"
#include "schedule/lockqueue.h"
#include "lock/lock.h"

void LockQueue::insert_into_queue(Txn* t, LockType type) {
  auto prep_stage_and_insert = [&t, &type, this]() {
    std::shared_ptr<LockStage> stage = 
      std::make_shared<LockStage>(std::unordered_set<Txn*>{t}, type);
    
    if (current == nullptr) {
      // no request present at the time of insertion
      current = stage;
      newest = stage;
      return;
    }

    // queue is not empty.
    newest->set_next_stage(stage);
    newest = stage;
  };

  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  if (type == LockType::exclusive) {
    prep_stage_and_insert();
    return;
  }

  // shared lock request!
  // attempt to add t to newest if that exists
  if (newest != nullptr) {
    if (newest->add_to_stage(t, type)) {
      return;
    }
  }

  // that failed, must add a stage
  prep_stage_and_insert();
}

std::unordered_set<Txn*> LockQueue::finalize_txn() {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  
  std::unordered_set<Txn*> ready_txns;
  if (current->decrement_holders() == 0) {
    // the current stage has been fully processed
    ASSERT(current->get_current_holders() == 0);

    if (current->get_next_request() != nullptr) {
      for(const auto& txn_pt : current->get_next_request()->get_requesters()) {
        if (txn_pt->lock_granted()) {
          // txn has all the locks needed;
          ready_txns.insert(txn_pt);
        }
      }
    }

    // move the current to the next
    if (current == newest) {
      newest = current->get_next_request();
      ASSERT(newest == nullptr);
    }
    current = current->get_next_request();
  }

  return ready_txns;
}

bool LockQueue::merge_into_lock_queue(std::shared_ptr<LockQueue> lq_ptr) {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  MutexRWGuard lq_read_lock(&lq_ptr->mutex_, LockType::shared);

  if (current == nullptr) {
    current = lq_ptr->current;
    newest = lq_ptr->newest;
    return true;
  }

  bool locks_granted = false;
  // coalesce the newest with lq_ptr->current if both are shared
  if (lq_ptr->current &&
      lq_ptr->current->get_lock_type() == LockType::shared &&
      newest->get_lock_type() == LockType::shared) {
    // insert all the txn in lq_ptr.current into newest
    for (auto& txn_ptr : lq_ptr->current->get_requesters()) {
      if(newest->add_to_stage(txn_ptr, LockType::shared) == false) {
        ASSERT(false);
      }
    }
    
    // move the pointer
    if (lq_ptr->newest == lq_ptr->current)
      lq_ptr->newest = lq_ptr->newest->get_next_request();
    lq_ptr->current = lq_ptr->current->get_next_request();

    // at coalescence, locks are granted if the stage 
    // being coalesced into is also current!
    locks_granted = (newest == current);
  }

  // newest is only moved if lq_ptr is non-null!
  newest->set_next_stage(lq_ptr->current);
  newest = lq_ptr->current == nullptr ? newest : lq_ptr->current;
  return locks_granted;
}

std::unordered_set<Txn*> LockQueue::signal_lock_granted() {
  std::unordered_set<Txn*> ready_txns;
  for (const auto& elt : current->get_requesters()) {
    if(elt->lock_granted()) {
      // all locks for txn granted
      ready_txns.insert(elt);
    }
  }

  return ready_txns;
}
