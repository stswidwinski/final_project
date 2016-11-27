#ifndef _LOCK_TABLE_H_
#define _LOCK_TABLE_H_

#include "schedule/lockqueue.h"
#include "txns/txn.h"
#include "lock/lock.h"

#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include <list>
#include <unordered_map>
#include <memory>

class LockTable {
private:
  struct ReadyTxnQueue {
    std::list<std::shared_ptr<Txn>> ready_queue;
    std::mutex mutex_;
    std::condition_variable cv_;
    
    // blocks if necessary
    void add_txn(std::shared_ptr<Txn> t);
    // blocks until a txn is available
    std::shared_ptr<Txn> get_ready_txn();
    // non-blocking version. Returns nullptr when nothing is left.
    std::shared_ptr<Txn> try_get_ready_txn();
  };

  // returns the pointer to the lock queue corresponding to
  // lck. Inserts the lock queue to the lock table if needed.
  std::shared_ptr<LockQueue> get_lock_queue(int lck);

  struct ReadyTxnQueue ready_queue;
  std::unordered_map<int, std::shared_ptr<LockQueue>> lock_table;
  MutexRW mutex_;

public:
  std::shared_ptr<Txn> get_next_ready_txn();
  std::shared_ptr<Txn> try_get_next_ready_txn();
  // Inserts lock request into the lock table without granting locks. 
  // Used by the batch schedule only.
  void insert_lock_request(std::shared_ptr<Txn> t, int lck, LockType type);
  void finalize_lock_request(int lck);
  // Merge lt into this lock table. Grants locks if appropriate.
  void merge_into_lock_table(LockTable& lt);
  // Two LockTables are equal if 
  //    1) they contain the same elements
  //      and the lockqueues are equal for every elements
  //    2) the ready queue contains the same elements in the
  //      same order.
  //
  // NOTE:
  //    THIS FUNCTION IS NOT THREAD SAFE. USE IN SINGLE-THREAD
  //    TESTS ONLY.
  bool operator==(const LockTable& lt) const;
  bool operator!=(const LockTable& lt) const;

  friend void InsertLockRequestTest();
  friend void FinalizeLockRequestTest(); 
  friend void MergeTest();
  friend void BatchScheduleInsert();
  friend void ScheduleMergingIntoEmptyTest();
  friend void ScheduleMergingIntoExistingTest();
  friend void BatchScheduleCreationTest1();
  friend void BatchScheduleCreationTest2();

  friend class ScheduleSnooper;
};

#endif // _LOCK_TABLE_H_
