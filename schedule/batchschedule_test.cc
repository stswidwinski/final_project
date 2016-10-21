#include "lock/lock.h"
#include "schedule/batchschedule.h"
#include "utils/testing.h"
#include "schedule/locktable.h"

TEST(BatchScheduleInsert) {
  BatchSchedule bs;

  Txn test_txn_1(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));

  Txn test_txn_2(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 3})));


  bs.add_txn(&test_txn_1);
  bs.add_txn(&test_txn_2);
  LockTable& lt = bs.get_lock_table();
  // The lock table for lock # 1 set properly
  EXPECT_EQ(
    lt.lock_table.find(1)->second->current->next_request,
    lt.lock_table.find(1)->second->newest);
  EXPECT_UNEQ(
    lt.lock_table.find(1)->second->current,
    lt.lock_table.find(1)->second->newest);
  EXPECT_EQ(
    lt.lock_table.find(1)->second->newest->requesters.size(), 
    1);
  EXPECT_TRUE(
    lt.lock_table.find(1)->second->newest->type == LockType::exclusive);
  EXPECT_TRUE(
    lt.lock_table.find(1)->second->newest->requesters.find(&test_txn_2) !=
    lt.lock_table.find(1)->second->newest->requesters.end());
  EXPECT_EQ(
    lt.lock_table.find(1)->second->current->requesters.size(), 
    1);
  EXPECT_TRUE(
    lt.lock_table.find(1)->second->current->requesters.find(&test_txn_1) !=
    lt.lock_table.find(1)->second->current->requesters.end());
  EXPECT_TRUE(
    lt.lock_table.find(1)->second->current->type == LockType::exclusive);
    
  // The lock table for lock # 2 set properly
  EXPECT_EQ(
    lt.lock_table.find(2)->second->current,
    lt.lock_table.find(2)->second->newest);
  EXPECT_EQ(
    lt.lock_table.find(2)->second->current->requesters.size(),
    2);
  EXPECT_TRUE(
    lt.lock_table.find(2)->second->current->type ==
    LockType::shared);
  EXPECT_TRUE(
    lt.lock_table.find(2)->second->current->requesters.find(&test_txn_1) !=
    lt.lock_table.find(2)->second->current->requesters.end());
  EXPECT_TRUE(
    lt.lock_table.find(2)->second->current->requesters.find(&test_txn_2) !=
    lt.lock_table.find(2)->second->current->requesters.end());

  // The lock table for lock # 3 set properly.
  EXPECT_UNEQ(
    lt.lock_table.find(3)->second->current,
    lt.lock_table.find(3)->second->newest);
  EXPECT_EQ(
    lt.lock_table.find(3)->second->current->requesters.size(),
    1);
  EXPECT_TRUE(
    lt.lock_table.find(3)->second->current->type ==
    LockType::exclusive);
  EXPECT_TRUE(
    lt.lock_table.find(3)->second->current->requesters.find(&test_txn_1) !=
    lt.lock_table.find(3)->second->current->requesters.end());
  EXPECT_EQ(
    lt.lock_table.find(3)->second->newest->requesters.size(),
    1);
 EXPECT_TRUE(
    lt.lock_table.find(3)->second->newest->type ==
    LockType::shared);
  EXPECT_TRUE(
    lt.lock_table.find(3)->second->newest->requesters.find(&test_txn_2) !=
    lt.lock_table.find(3)->second->newest->requesters.end());
  
  END; 
}

int main(int argc, char** argv) {
  BatchScheduleInsert();
  return 0;
}
