#include "schedule/locktable/readytxnqueue.h"

void ReadyTxnQueue::add_txn(std::shared_ptr<Txn> t) {
  std::unique_lock<std::mutex> lck(mutex_);
  ready_queue.push_back(t);
  cv_.notify_all();
}

std::shared_ptr<Txn> ReadyTxnQueue::get_ready_txn() {
  std::unique_lock<std::mutex> lck(mutex_);
  while (ready_queue.size() == 0) {
    cv_.wait(lck);
  }

  std::shared_ptr<Txn> t = ready_queue.front();
  ready_queue.pop_front();
  return t;
}

std::shared_ptr<Txn> ReadyTxnQueue::try_get_ready_txn() {
  std::unique_lock<std::mutex> lck(mutex_);
  if (ready_queue.size() == 0)
    return nullptr;

  std::shared_ptr<Txn> t = ready_queue.front();
  ready_queue.pop_front();
  return t;
}
