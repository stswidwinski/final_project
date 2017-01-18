#ifndef _TEST_LOCK_QUEUE_H_
#define _TEST_LOCK_QUEUE_H_

class TestLockQueue : public LockQueue {
  // A bunch of friend test functions.
  friend void ExclusiveTxnQueueingTest(); 
  friend void SharedTxnQueueingTest();
  friend void SharedAndExclusiveQueueingTest();
  friend void FinalizeTxnTest();
  friend void FinalizeTxnMultiLockTest();
  friend void SignalLockGrantedTest();
  friend void SimpleMergingTest();
};

#endif // _TEST_LOCK_QUEUE_H_
