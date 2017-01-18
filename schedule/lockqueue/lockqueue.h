#ifndef _LOCK_QUEUE_H_
#define _LOCK_QUEUE_H_

#include "schedule/lockstage/lockstage.h"
#include "utils/mutex.h"
#include "lock/lock.h"

#include <memory>
#include <unordered_set>

class LockQueue {
protected:
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

  friend bool operator==(const LockQueue& lq1, const LockQueue& lq2);

  friend void BatchScheduleInsert();
  
  friend class ScheduleSnooper;
};

#endif // _LOCK_QUEUE_H_
