#include "schedule/locktable.h"
#include "utils/testing.h"
#include "txns/txn.h"
#include "lock/lock.h"

Txn test_txn(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1})),
  std::shared_ptr<std::set<int>>(new std::set<int>({2})));

// NOTE:
//    Lock request insertion is so closely coupled witht he underlying
//    LockQueue that the main point of the test is to check whether
//    entryies in lock table are created. The insertion is implicitly
//    tested when finalization is checked.
TEST(InsertLockRequestTest){
  LockTable lt;

  lt.insert_lock_request(&test_txn, 1, LockType::exclusive);
  EXPECT_TRUE(lt.lock_table.find(1) != lt.lock_table.end());
  EXPECT_EQ(1, lt.lock_table.size());

  lt.insert_lock_request(&test_txn, 2, LockType::shared);
  EXPECT_TRUE(lt.lock_table.find(2) != lt.lock_table.end());
  EXPECT_EQ(2, lt.lock_table.size());

  END;
}

TEST(FinalizeLockRequestTest){
  LockTable lt;
  Txn fake_txn(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));   
  Txn txn(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));
  Txn txn2(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({})));

  // since insert lock request does not grant locks formally,
  // we must insert a fake txn to "simulate" granted locks.
  lt.insert_lock_request(&fake_txn, 1, LockType::exclusive);
  lt.insert_lock_request(&fake_txn, 2, LockType::exclusive);
  lt.insert_lock_request(&txn, 1, LockType::exclusive);
  lt.insert_lock_request(&txn, 2, LockType::shared);
  lt.insert_lock_request(&txn2, 1, LockType::exclusive);

  // no transaction should be ready to go.
  lt.finalize_lock_request(1);
  EXPECT_EQ(0, lt.ready_queue.ready_queue.size());
 
  // txn got both locks.
  lt.finalize_lock_request(2);
  EXPECT_EQ(1, lt.ready_queue.ready_queue.size());
  EXPECT_EQ(&txn, lt.get_next_ready_txn());

  lt.finalize_lock_request(1);
  EXPECT_EQ(1, lt.ready_queue.ready_queue.size());
  EXPECT_EQ(&txn2, lt.get_next_ready_txn());

  END;
}

int main(int argc, char** argv) {
  InsertLockRequestTest();
  FinalizeLockRequestTest();
}
