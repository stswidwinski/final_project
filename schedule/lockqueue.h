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
  LockQueue() {};
  LockQueue(std::shared_ptr<LockStage> cur):
    current(cur),
    newest(cur) 
  {}

  LockQueue(std::shared_ptr<LockStage> cur, std::shared_ptr<LockStage> newe):
    current(cur),
    newest(newe)
  {}

  void insert_into_queue(std::shared_ptr<Txn> t, LockType type);
  // Returns the set of txns that have all the required locks
  // after the current stage has been finalized.
  std::unordered_set<std::shared_ptr<Txn>> finalize_txn();
  // Returns true if merge results in granting locks.
  //
  // Does NOT change lq_ptr in any persistent way.
  bool merge_into_lock_queue(std::shared_ptr<LockQueue> lq_ptr);
  // signals all txns within current LockStage about lock being
  // granted
  //
  // Returns the set of transactions that have received
  // all of its locks.
  std::unordered_set<std::shared_ptr<Txn>> signal_lock_granted(); 

  // getters
  std::shared_ptr<LockStage> getCurrent() const;
  std::shared_ptr<LockStage> getNewest() const;

  // A bunch of friend test functions.
  friend void ExclusiveTxnQueueingTest(); 
  friend void SharedTxnQueueingTest();
  friend void SharedAndExclusiveQueueingTest();
  friend void FinalizeTxnTest();
  friend void FinalizeTxnMultiLockTest();
  friend void SignalLockGrantedTest();
  friend void SimpleMergingTest();
  friend void BatchScheduleInsert();

  friend class ScheduleSnooper;
};

#endif // _LOCK_QUEUE_H_
