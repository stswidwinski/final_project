#include "schedule/schedule.h"
#include "utils/debug.h"

// TODO: profile and decide whether holders should be managed using 
//    CAS / FAI or other hardware atomic instruction
bool Schedule::LockStage::add_to_stage(Txn* txn, LockType req_type){
  if (req_type == LockType::exclusive ||
      type == LockType::exclusive) 
    return false;

  requesters.insert(txn);
  holders ++;
  return true;
}

int Schedule::LockStage::decrement_holders() {
  ASSERT(holders > 0);

  return (-- holders);
}

void Schedule::LockStage::set_next_stage(std::shared_ptr<struct Schedule::LockStage> ls) {
  ASSERT(next_request == nullptr);

  next_request = ls;
}

bool Schedule::LockQueue_::insert_into_queue(std::shared_ptr<struct Schedule::LockStage> new_stage) {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);

  if (current == nullptr) {
    // no requests for lock present at all
    current = new_stage;
    newest = new_stage;
    return true;
  }

  // requests present. append to tail.
  newest->set_next_stage(new_stage);
  newest = new_stage;
  return false;
}

bool Schedule::LockQueue_::merge_into_lock_queue(struct Schedule::LockQueue_& lq) {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  MutexRWGuard lq_write_lock(&lq.mutex_, LockType::exclusive);

  if (current == nullptr) {
    current = lq.current;
    newest = lq.newest;
    return true;
  }

  newest->next_request = lq.current;
  newest = lq.current;
  return false;
}

std::shared_ptr<struct Schedule::LockQueue_> Schedule::LockQueues::get_lock_queue(int lck) {
  MutexRWGuard read_lock(&mutex_, LockType::shared);
  std::shared_ptr<struct Schedule::LockQueue_> res = nullptr;

  auto lock_q_it = lock_queues.find(lck);
  if (lock_q_it != lock_queues.end())
    res = (*lock_q_it).second;

  return res;
}

std::unordered_set<Txn*> Schedule::LockQueue_::finalize_txn_within_current() {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  if(current->decrement_holders() == 0) {
    // we must grant new locks and go further with the queue!
    ASSERT(current->holders == 0);

    std::unordered_set<Txn*> ready_txns;
    for (auto txn : current->next_request->requesters) {
      if (txn->lock_granted()) {
        // txn has all its locks.
        ready_txns.insert(txn);
      }
    }

    // move the current to the next!
    if (current == newest) {
      newest = current->next_request;
    }
    current = current->next_request;

    return ready_txns;
  }

  return std::unordered_set<Txn*> {};
}

std::shared_ptr<struct Schedule::LockQueue_> Schedule::LockQueues::emplace_lock_queue(int lck) {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  // NOTE:
  //    There is no race condition here. The following only inserts if there is none 
  //    at the time of execution. So no overwritting happens.
  return lock_queues.emplace(lck, std::make_shared<struct LockQueue_>()).first->second;
}

bool Schedule::LockQueues::insert_ex_lock_request(Txn* t, int lck) {
  // prepare the new elt
  std::shared_ptr<struct LockStage> new_stage = std::make_shared<struct LockStage>();   
  new_stage->holders ++;
  new_stage->requesters.insert(t);
  new_stage->type = LockType::exclusive;

  // obtain pointer to the lock queue
  auto lock_q_pt = get_lock_queue(lck);
  if (lock_q_pt == nullptr) 
    lock_q_pt = emplace_lock_queue(lck);

  return lock_q_pt->insert_into_queue(new_stage);
}

bool Schedule::LockQueues::insert_sh_lock_request(Txn* t, int lck) { 
  // try to find among all the lock queues
  auto lock_q_pt = get_lock_queue(lck);
  if (lock_q_pt == nullptr)
    lock_q_pt = emplace_lock_queue(lck);

  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  // attempt to add t newest if that exists.
  if (lock_q_pt->newest != nullptr) {
    if (lock_q_pt->newest->add_to_stage(t, LockType::shared)) {
      return (lock_q_pt->newest == lock_q_pt->current);
    }
  }
  write_lock.Unlock();

  // if that failed, must add a stage
  std::shared_ptr<struct LockStage> new_stage = std::make_shared<struct LockStage>();
  new_stage->holders ++;
  new_stage->requesters.insert(t);

  return lock_q_pt->insert_into_queue(new_stage);
}

void Schedule::ReadyTxnQueue::add_txn(Txn *t) {
  std::unique_lock<std::mutex> lck(mutex_);
  ready_txns.push_back(t);
  cv_.notify_all();
}

Txn* Schedule::ReadyTxnQueue::get_ready_txn() {
  std::unique_lock<std::mutex> lck(mutex_);
  while (ready_txns.size() == 0) {
    cv_.wait(lck);
  }
  auto t = ready_txns.front();
  ready_txns.pop_front();
  return t;
}

void Schedule::LockQueues::finalize_lock_request(int lck) {
  auto lock_q_pt = get_lock_queue(lck);
  ASSERT(lock_q_pt != nullptr);

  auto ready_transactions = lock_q_pt->finalize_txn_within_current();
  // only non-batch schedules "ready" transactions!
  //
  // TODO:
  //    This function should only ever be called from non-batch schedule any way.
  //    Confirm this and clean up.
 // if (!is_batch_schedule) {
    for (auto& t : ready_transactions) {
      if (t->lock_granted()) {
        // all locks for transaction have been granted!
        ready_txns.add_txn(t);
      }
    }
 // }
}

void Schedule::add(Txn *t) {
  auto wh = t->get_write_set_handle();
  auto rh = t->get_read_set_handle();
  
  // locks are processed in order! Hence, there will be no deadlock!
  // TODO:
  //    Make the locks sorted. That should be rather fast since its done before 
  //    the txn is submitted so we don't care. W/e.
  //
  //    Might mean making the sets a vector. Binary search still gives good performance!

  // TODO:
  //    This is only called for batch schedules. Non-batch schedules get created via merge.
  //    Hence, it seems we NEVER actually grant locks here. Mare sure it is true. And 
  //    delete this lock_granted() stuff if so!
  //
  // Notify transactions if the lock is inmediately granted.
  for (auto lck : (*wh)) {
   // if (is_batch_schedule == false &&
        locks_.insert_ex_lock_request(t, lck); // == true
          //) {
   //   t->lock_granted();
  //  }
  }

  for (auto lck : (*rh)) {
   // if (is_batch_schedule == false &&
        locks_.insert_sh_lock_request(t, lck);//  == true) {
   //   t->lock_granted();
   // }
  }
}

void Schedule::finalize(Txn *t) {
  // release the locks.
  auto wh = t->get_write_set_handle();
  auto rh = t->get_write_set_handle();

  for (auto lck : (*wh)) {
    locks_.finalize_lock_request(lck);    
  }

  for (auto lck : (*rh)) {
    locks_.finalize_lock_request(lck);
  }
}

Txn* Schedule::get_next_txn_to_execute() {
   return locks_.ready_txns.get_ready_txn();
}

void Schedule::merge_into_schedule(Schedule& s) {
  ASSERT(s.is_batch_schedule == true);
  ASSERT(s.locks_.ready_txns.ready_txns.size() == 0);
  ASSERT(this->is_batch_schedule == false);
  
  for (auto& elt : s.locks_.lock_queues) {
    // obtain pointer to the lock queue
    auto lock_q_pt = locks_.get_lock_queue(elt.first);
    if (lock_q_pt == nullptr) 
      lock_q_pt = locks_.emplace_lock_queue(elt.first);

    if (lock_q_pt->merge_into_lock_queue(*lock_q_pt) == true) {
      for (auto& txn : lock_q_pt->current->requesters) {
        if (txn->lock_granted() == 0) {
          // all locks for this txn have been granted!
          locks_.ready_txns.add_txn(txn); 
        }
      }
    }
  }
}
