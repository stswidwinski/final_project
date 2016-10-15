#ifndef _LOCK_QUEUE_H_
#define _LOCK_QUEUE_H_

#include "schedule/lockstage.h"
#include "utils/mutex.h"
#include "lock/lock.h"

#include <memory>
#include <unordered_set>

class LockQueue {
private:
  std::shared_ptr<LockStage> current;
  std::shared_ptr<LockStage> newest;
  MutexRW mutex_;
public:
  void insert_into_queue(Txn* t, LockType type);
  // Returns the set of txns that have all the required locks
  // after the current stage has been finalized.
  std::unordered_set<Txn*> finalize_txn();
  // Returns true if merge results in granting locks.
  bool merge_into_lock_queue(std::shared_ptr<LockQueue> lq_ptr);
  // signals all txns within current LockStage about lock being
  // granted
  //
  // Returns the set of transactions that have received
  // all of its locks.
  std::unordered_set<Txn*> signal_lock_granted(); 

  // A bunch of friend test functions.
  friend void ExclusiveTxnQueueingTest(); 
  friend void SharedTxnQueueingTest();
  friend void SharedAndExclusiveQueueingTest();
  friend void FinalizeTxnTest();
  friend void FinalizeTxnMultiLockTest();
  friend void SignalLockGrantedTest();
};

#endif // _LOCK_QUEUE_H_
