#include "schedule/locktable/locktable.h"
#include "schedule/locktable/locktable_test_utils.h"
#include "schedule/locktable/testlocktable.h"
#include "utils/testing.h"
#include "txns/txn.h"
#include "lock/lock.h"

std::shared_ptr<Txn> test_txn = std::make_shared<Txn>(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1})),
  std::shared_ptr<std::set<int>>(new std::set<int>({2})));

std::shared_ptr<Txn> test_txn_2 = std::make_shared<Txn>(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1})),
  std::shared_ptr<std::set<int>>(new std::set<int>({2})));

// NOTE:
//    Lock request insertion is so closely coupled witht he underlying
//    LockQueue that the main point of the test is to check whether
//    entryies in lock table are created. The insertion is implicitly
//    tested when finalization is checked.
TEST(InsertLockRequestTest){
  TestLockTable lt;

  lt.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_TRUE(lt.lock_table.find(1) != lt.lock_table.end());
  EXPECT_EQ(1, lt.lock_table.size());

  lt.insert_lock_request(test_txn, 2, LockType::shared);
  EXPECT_TRUE(lt.lock_table.find(2) != lt.lock_table.end());
  EXPECT_EQ(2, lt.lock_table.size());

  END;
}

TEST(FinalizeLockRequestTest){
  TestLockTable lt;
  std::shared_ptr<Txn> fake_txn = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));   
  std::shared_ptr<Txn> txn = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));
  std::shared_ptr<Txn> txn2 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));

  // since insert lock request does not grant locks formally,
  // we must insert a fake txn to "simulate" granted locks.
  lt.insert_lock_request(fake_txn, 1, LockType::exclusive);
  lt.insert_lock_request(fake_txn, 2, LockType::exclusive);
  lt.insert_lock_request(txn, 1, LockType::exclusive);
  lt.insert_lock_request(txn, 2, LockType::shared);
  lt.insert_lock_request(txn2, 1, LockType::exclusive);

  // no transaction should be ready to go.
  lt.finalize_lock_request(1);
  EXPECT_EQ(0, lt.ready_queue.ready_queue.size());
 
  // txn got both locks.
  lt.finalize_lock_request(2);
  EXPECT_EQ(1, lt.ready_queue.ready_queue.size());
  EXPECT_EQ(txn, lt.get_next_ready_txn());

  lt.finalize_lock_request(1);
  EXPECT_EQ(1, lt.ready_queue.ready_queue.size());
  EXPECT_EQ(txn2, lt.get_next_ready_txn());

  END;
}

TEST(MergeTest) {
  TestLockTable lt1;
  TestLockTable lt2;
  std::shared_ptr<Txn> txn = std::make_shared<Txn>( 
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));
  std::shared_ptr<Txn> txn2 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));
  std::shared_ptr<Txn> txn3 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));

  // lt1 is the table we will be merging into.
  lt1.insert_lock_request(txn, 1, LockType::shared);
  lt1.insert_lock_request(txn, 2, LockType::exclusive);
  lt1.insert_lock_request(txn, 3, LockType::exclusive);
  
  lt2.insert_lock_request(txn2, 1, LockType::shared);
  lt2.insert_lock_request(txn, 2, LockType::exclusive);
  lt2.insert_lock_request(txn3, 3, LockType::shared); 

  lt1.merge_into_lock_table(lt2);
  // txn2 should be in the ready queue
  EXPECT_EQ(1, lt1.ready_queue.ready_queue.size());
  EXPECT_EQ(txn2, lt1.get_next_ready_txn());

  END;
}

TEST(EqualityTest) {
  TestLockTable lt1;
  TestLockTable lt2;

  // basic insertion of a single exclusive lock.
  EXPECT_TRUE(lt1 == lt2);
  lt1.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_FALSE(lt1 == lt2);
  lt2.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_TRUE(lt1 == lt2);

  // insertion of a different lock. 
  lt1.insert_lock_request(test_txn, 2, LockType::exclusive);
  EXPECT_FALSE(lt1 == lt2);
  lt2.insert_lock_request(test_txn, 2, LockType::exclusive);
  EXPECT_TRUE(lt1 == lt2);
  
  // basic insertion of multiple exclusive locks
  lt1.insert_lock_request(test_txn, 1, LockType::exclusive);
  lt1.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_FALSE(lt1 == lt2);
  lt2.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_FALSE(lt1 == lt2);
  lt2.insert_lock_request(test_txn, 1, LockType::exclusive);
  EXPECT_TRUE(lt1 == lt2);

  TestLockTable lt3;
  TestLockTable lt4;

  // basic insertion of a single shared lock.
  lt3.insert_lock_request(test_txn, 1, LockType::shared);
  EXPECT_FALSE(lt3 == lt4);
  lt4.insert_lock_request(test_txn, 1, LockType::shared);
  EXPECT_TRUE(lt3 == lt4);

  // basic insertion of different test_txns
  lt3.insert_lock_request(test_txn_2, 1, LockType::shared);
  EXPECT_FALSE(lt3 == lt4);
  lt4.insert_lock_request(test_txn_2, 1, LockType::shared);
  EXPECT_TRUE(lt3 == lt4);
  
  END;
}

int main(int argc, char** argv) {
  InsertLockRequestTest();
  FinalizeLockRequestTest();
  MergeTest();
  EqualityTest();
}
