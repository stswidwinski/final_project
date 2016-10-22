#include "schedule/locktable.h"
#include "utils/debug.h"

void LockTable::ReadyTxnQueue::add_txn(Txn* t) {
  std::unique_lock<std::mutex> lck(mutex_);
  ready_queue.push_back(t);
  cv_.notify_all();
}

Txn* LockTable::ReadyTxnQueue::get_ready_txn() {
  std::unique_lock<std::mutex> lck(mutex_);
  while (ready_queue.size() == 0) {
    cv_.wait(lck);
  }

  Txn* t = ready_queue.front();
  ready_queue.pop_front();
  return t;
}

// NOTE:
//  We can release the lock after we have obtained the 
//  queue because the lock table changes ONLY when 
//  we insert lock queues. Once a lock queue is present,
//  it does not change addresses etc.
std::shared_ptr<LockQueue> LockTable::get_lock_queue(int lck) {
  MutexRWGuard read_lock(&mutex_, LockType::shared);

  auto lock_q_it = lock_table.find(lck);
  if (lock_q_it != lock_table.end())
    return (*lock_q_it).second;

  // lock queue has not been found in lock table!
  read_lock.Unlock();

  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  // NOTE
  //  There is no race condition here, since emplace only
  //  inserts the element if it does not exist at the time
  //  of execution.
  return lock_table.emplace(lck, std::make_shared<LockQueue>()).first->second;
}

bool LockTable::operator==(const LockTable& lt) const {
  // we must have the same number of elt in the lock tables.
  if (lock_table.size() != lt.lock_table.size())
    return false;

  // the ready queues must have the same size.
  if (ready_queue.ready_queue.size() != lt.ready_queue.ready_queue.size())
    return false;

  // every element in lock_table must be present in lts lock table 
  // and map to the same elt.
  for (auto entry : lock_table) {
    if (lt.lock_table.find(entry.first) == lt.lock_table.end())
      return false;

    if ((*entry.second) != (*lt.lock_table.find(entry.first)->second))
      return false;
  }

  // every elt in ready queue must be equivalent.
  auto usIt = ready_queue.ready_queue.begin();
  auto theyIt = lt.ready_queue.ready_queue.begin();
  for (; usIt != ready_queue.ready_queue.end(); usIt++, theyIt++) {
    if (*usIt != *theyIt)
      return false;
  }

  return true;
}

bool LockTable::operator!=(const LockTable& lt) const {
  return !(LockTable::operator==(lt));
}

void LockTable::insert_lock_request(Txn* t, int lck, LockType type) {
  get_lock_queue(lck)->insert_into_queue(t, type);
}

void LockTable::finalize_lock_request(int lck) {
  auto lock_q_pt = get_lock_queue(lck);
  ASSERT(lock_q_pt != nullptr);

  auto ready_txns = lock_q_pt->finalize_txn();
  for (auto& txn_ptr : ready_txns) {
    if (txn_ptr->has_all_locks()) {
      // all locks for this txn have been grantes
      ready_queue.add_txn(txn_ptr);
    }
  }
}

void LockTable::merge_into_lock_table(LockTable &t) {
  ASSERT(t.ready_queue.ready_queue.size() == 0);

  // merge each of the lock table entries
  for (auto& elt : t.lock_table) {
    auto lock_q_pt = get_lock_queue(elt.first);
    if(lock_q_pt->merge_into_lock_queue(elt.second)) {
      // we must signal all the txns about locks granted!
      auto ready_txns = elt.second->signal_lock_granted();
      for (auto& txn_ptr : ready_txns) {
        ready_queue.add_txn(txn_ptr);
      }
    }
  }
}

Txn* LockTable::get_next_ready_txn() {
  return ready_queue.get_ready_txn();
}
