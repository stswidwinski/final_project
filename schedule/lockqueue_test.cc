#include "lock/lock.h"
#include "schedule/lockqueue.h"
#include "schedule/lockqueue_test_utils.h"
#include "schedule/testlockqueue.h"
#include "txns/txn.h"
#include "utils/testing.h"

#include <memory>
#include <set>

std::shared_ptr<Txn> test_txn = std::make_shared<Txn>(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1})),
  std::shared_ptr<std::set<int>>(new std::set<int>({2})));

std::shared_ptr<Txn> test_txn2 = std::make_shared<Txn>(
  1,
  std::shared_ptr<std::set<int>>(new std::set<int>({1})),
  std::shared_ptr<std::set<int>>(new std::set<int>({2})));

TEST(ExclusiveTxnQueueingTest) {
  TestLockQueue lq;
  EXPECT_TRUE(lq.current == nullptr);
  EXPECT_TRUE(lq.newest == nullptr);
  
  // insert exclusive lock.
  lq.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_EQ(1, lq.current->get_requesters().size());

  // insert another exclusive lock
  lq.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq.current != lq.newest);
  EXPECT_EQ(1, lq.current->get_requesters().size());
  EXPECT_EQ(1, lq.newest->get_requesters().size());

  END;
}

TEST(SharedTxnQueueingTest) {
  TestLockQueue lq;
  EXPECT_TRUE(lq.current == nullptr);
  EXPECT_TRUE(lq.newest == nullptr);

  // insert shared lock
  lq.insert_into_queue(test_txn, LockType::shared);
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_EQ(1, lq.current->get_requesters().size());

  // insert another one
  lq.insert_into_queue(test_txn2, LockType::shared);
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_EQ(2, lq.current->get_requesters().size());

  END; 
}

TEST(SharedAndExclusiveQueueingTest) {
  TestLockQueue lq1;
  TestLockQueue lq2;
  EXPECT_TRUE(lq1.current == nullptr);
  EXPECT_TRUE(lq1.newest == nullptr);
  EXPECT_TRUE(lq2.current == nullptr);
  EXPECT_TRUE(lq2.newest == nullptr);

  // insert shared lock
  lq1.insert_into_queue(test_txn, LockType::shared);
  EXPECT_TRUE(lq1.current == lq1.newest);
  EXPECT_EQ(1, lq1.current->get_requesters().size());

  // insert exclusive lock
  lq1.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq1.current != lq1.newest);
  EXPECT_EQ(1, lq1.current->get_requesters().size());
  EXPECT_TRUE(LockType::shared == lq1.current->get_lock_type());
  EXPECT_EQ(1, lq1.newest->get_requesters().size());
  EXPECT_TRUE(LockType::exclusive == lq1.newest->get_lock_type());

  // TEST THE OPPOSITE ORDER
  // insert exclusive lock
  lq2.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq2.current == lq2.newest);
  EXPECT_EQ(1, lq2.current->get_requesters().size());

  // insert shared lock
  lq2.insert_into_queue(test_txn, LockType::shared);
  EXPECT_TRUE(lq2.current != lq2.newest);
  EXPECT_EQ(1, lq2.current->get_requesters().size());
  EXPECT_TRUE(LockType::exclusive == lq2.current->get_lock_type());
  EXPECT_EQ(1, lq2.newest->get_requesters().size());
  EXPECT_TRUE(LockType::shared == lq2.newest->get_lock_type());

  END; 
}

// Finalization of txns returns transactions with a single lock request
// and move on the queue.
TEST(FinalizeTxnTest) {
  TestLockQueue lq;

  std::shared_ptr<Txn> finalize_txn_1 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));

  // shared lock finalize
  lq.insert_into_queue(finalize_txn_1, LockType::shared);
  auto return_val = lq.finalize_txn();
  EXPECT_EQ(0, return_val.size());
  EXPECT_TRUE(nullptr == lq.current);
  EXPECT_TRUE(nullptr == lq.newest);

  std::shared_ptr<Txn> finalize_txn_2 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({})),
    std::shared_ptr<std::set<int>>(new std::set<int>({1})));

  // exclusive lock finalize
  lq.insert_into_queue(finalize_txn_2, LockType::exclusive);
  return_val = lq.finalize_txn();
  EXPECT_EQ(0, return_val.size());
  EXPECT_TRUE(nullptr == lq.current);
  EXPECT_TRUE(nullptr == lq.newest);

  // finalization with next lock stage.
  std::shared_ptr<Txn> finalize_txn_3 = std::make_shared<Txn>(  
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));
  std::shared_ptr<Txn> finalize_txn_4 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));
  std::shared_ptr<Txn> finalize_txn_5 = std::make_shared<Txn>( 
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));
  lq.insert_into_queue(finalize_txn_3, LockType::exclusive);
  lq.insert_into_queue(finalize_txn_4, LockType::shared);
  lq.insert_into_queue(finalize_txn_5, LockType::shared);
  return_val = lq.finalize_txn();
  EXPECT_EQ(2, return_val.size());
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_EQ(2, lq.current->get_requesters().size());

  // finalization should not return anything until all txns finalize.
  std::shared_ptr<Txn> finalize_txn_6 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));
  lq.insert_into_queue(finalize_txn_6, LockType::exclusive);

  return_val = lq.finalize_txn();
  EXPECT_EQ(0, return_val.size());
  EXPECT_TRUE(lq.current != lq.newest);

  return_val = lq.finalize_txn();
  EXPECT_EQ(1, return_val.size());
  EXPECT_TRUE(lq.current == lq.newest);

  END;
}

// For a transaction requiring multiple locks, granting a single lock 
// is not enough and the txn will not be returned, but the lock queue
// will go on.
TEST(FinalizeTxnMultiLockTest) {
  TestLockQueue lq; 
  std::shared_ptr<Txn> finalize_txn_1 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));

  lq.insert_into_queue(finalize_txn_1, LockType::exclusive);
  auto result = lq.finalize_txn();
  EXPECT_EQ(0, result.size());
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_TRUE(nullptr == lq.current);

  END;
}

// Signaling returns what is correct and does not iterate.
TEST(SignalLockGrantedTest) {
  TestLockQueue lq; 
  std::shared_ptr<Txn> finalize_txn_1 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));
  std::shared_ptr<Txn> finalize_txn_2 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));

  lq.insert_into_queue(finalize_txn_1, LockType::shared);
  lq.insert_into_queue(finalize_txn_2, LockType::shared);
  auto result = lq.signal_lock_granted();
  EXPECT_EQ(1, result.size());
  EXPECT_TRUE(lq.current == lq.newest);
  EXPECT_TRUE(nullptr != lq.current);

  END;
}

TEST(SimpleMergingTest) {
  TestLockQueue lq1;
  TestLockQueue lq2;

  lq1.insert_into_queue(test_txn, LockType::exclusive);
  lq2.insert_into_queue(test_txn2, LockType::shared);
  
  // merge without coalescing
  EXPECT_FALSE(lq2.merge_into_lock_queue(
        std::make_shared<LockQueue>(lq1)));
  EXPECT_TRUE(lq2.current != lq2.newest);
  EXPECT_EQ(1, lq2.current->get_requesters().size());
  EXPECT_EQ(1, lq2.newest->get_requesters().size());

  // merge with coalescing.
  TestLockQueue lq3;
  TestLockQueue lq4;

  lq3.insert_into_queue(test_txn, LockType::shared);
  lq4.insert_into_queue(test_txn2, LockType::shared);

  EXPECT_TRUE(lq3.merge_into_lock_queue(
        std::make_shared<LockQueue>(lq4)));
  EXPECT_EQ(2, lq3.current->get_requesters().size());
  EXPECT_TRUE(lq3.current == lq3.newest);
  EXPECT_TRUE(nullptr != lq3.current);

  // coalescing should not happen if shared lock is not the newest!
  TestLockQueue lq5;
  TestLockQueue lq6;

  lq5.insert_into_queue(test_txn, LockType::shared);
  lq5.insert_into_queue(test_txn, LockType::exclusive);
  lq6.insert_into_queue(test_txn2, LockType::shared);

  EXPECT_FALSE(lq5.merge_into_lock_queue(
        std::make_shared<LockQueue>(lq6)));
  EXPECT_TRUE(lq5.current->get_next_request()->get_next_request() == lq5.newest);

  // a merge with an empty lock queue does not cause problems
  TestLockQueue lq7;
  TestLockQueue lq8;
  
  lq7.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_FALSE(lq7.merge_into_lock_queue(
        std::make_shared<LockQueue>(lq8)));
  EXPECT_TRUE(lq7.current == lq7.newest);
  EXPECT_TRUE(lq7.current != nullptr);

  END;
}

TEST(EqualityTest) {
  TestLockQueue lq1;
  TestLockQueue lq2;

  EXPECT_TRUE(lq1 == lq2);

  // insert exclusive lock.
  lq1.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_FALSE(lq1 == lq2);
  lq2.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq1 == lq2);

  // insert another exclusive lock
  lq1.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_FALSE(lq1 == lq2);
  lq2.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq1 == lq2);

  // insert a share lock afterwards.
  lq1.insert_into_queue(test_txn, LockType::shared);
  EXPECT_FALSE(lq1 == lq2);
  lq2.insert_into_queue(test_txn, LockType::shared);
  EXPECT_TRUE(lq1 == lq2);

  // insert another exclusive lock afterwards
  lq1.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_FALSE(lq1 == lq2);
  lq2.insert_into_queue(test_txn, LockType::exclusive);
  EXPECT_TRUE(lq1 == lq2);
  
  END;
}
int main(int argc, char** argv) {
  ExclusiveTxnQueueingTest();
  SharedTxnQueueingTest();
  SharedAndExclusiveQueueingTest();
  FinalizeTxnTest();
  FinalizeTxnMultiLockTest();
  SignalLockGrantedTest();
  SimpleMergingTest();
  EqualityTest();
}
