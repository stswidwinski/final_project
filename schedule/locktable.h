#ifndef _LOCK_TABLE_H_
#define _LOCK_TABLE_H_

#include "schedule/lockqueue.h"
#include "schedule/readytxnqueue.h"
#include "txns/txn.h"
#include "lock/lock.h"

#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include <list>
#include <unordered_map>
#include <memory>

class LockTable {
protected:
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

  friend bool operator==(const LockTable& lt1, const LockTable& lt2);
  friend bool operator!=(const LockTable& lt1, const LockTable& lt2);

  friend void BatchScheduleInsert();
  friend void ScheduleMergingIntoEmptyTest();
  friend void ScheduleMergingIntoExistingTest();
  friend void BatchScheduleCreationTest1();
  friend void BatchScheduleCreationTest2();

  friend class ScheduleSnooper;
};

#endif // _LOCK_TABLE_H_
