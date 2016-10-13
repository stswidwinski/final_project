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
    std::list<Txn*> ready_queue;
    std::mutex mutex_;
    std::condition_variable cv_;
    
    // blocks if necessary
    void add_txn(Txn* t);
    // blocks until a txn is available
    Txn* get_ready_txn();
  };

  // returns the pointer to the lock queue corresponding to
  // lck. Inserts the lock queue to the lock table if needed.
  std::shared_ptr<LockQueue> get_lock_queue(int lck);

  struct ReadyTxnQueue ready_queue;
  std::unordered_map<int, std::shared_ptr<LockQueue>> lock_table;
  MutexRW mutex_;
public:
  Txn* get_next_ready_txn();
  void insert_lock_request(Txn* t, int lck, LockType type);
  void finalize_lock_request(int lck);
  void merge_into_lock_table(LockTable& lt);
};

#endif // _LOCK_TABLE_H_
