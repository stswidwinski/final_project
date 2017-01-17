#ifndef _READY_TXN_QUEUE_H_
#define _READY_TXN_QUEUE_H_

#include "txns/txn.h"

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

struct ReadyTxnQueue {
  std::list<std::shared_ptr<Txn>> ready_queue;
  std::mutex mutex_;
  std::condition_variable cv_;
  
  // blocks if necessary
  void add_txn(std::shared_ptr<Txn> t);
  // blocks until txn is available
  std::shared_ptr<Txn> get_ready_txn();
  // non-blocking version of the above. 
  // Returns nullptr if nothing available.
  std::shared_ptr<Txn> try_get_ready_txn();
};

#endif // _READY_TXN_QUEUE_H_
