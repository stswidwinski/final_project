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
  LockStage expected_newest_1 = LockStage(
    std::unordered_set<Txn*>{&test_txn_2},
    LockType::exclusive);
  LockStage expected_current_1 = LockStage(
    std::unordered_set<Txn*>{&test_txn_1},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_1));
  LockQueue expected_locktable_1 = LockQueue(
    std::make_shared<LockStage>(expected_current_1),
    std::make_shared<LockStage>(expected_newest_1));

  EXPECT_TRUE(*(lt.lock_table.find(1)->second) == expected_locktable_1);
  
  // The lock table for lock # 2 set properly
  LockStage expected_new_cur_2 = LockStage(
    std::unordered_set<Txn*>{&test_txn_1, &test_txn_2},
    LockType::shared);
  LockQueue expected_queue_2 = LockQueue(
    std::make_shared<LockStage>(expected_new_cur_2));

  EXPECT_TRUE(*(lt.lock_table.find(2)->second) == expected_queue_2);

  // The lock table for lock # 3 set properly.
  LockStage expected_newest_3 = LockStage(
    std::unordered_set<Txn*>{&test_txn_2},
    LockType::shared);
  LockStage expected_current_3 = LockStage(
    std::unordered_set<Txn*>{&test_txn_1},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_3));
  LockQueue expected_queue_3 = LockQueue(
    std::make_shared<LockStage>(expected_current_3),
    std::make_shared<LockStage>(expected_newest_3));

  EXPECT_TRUE(*(lt.lock_table.find(3)->second) == expected_queue_3);
  
  END; 
}

TEST(BatchScheduleCreation) {
  std::unique_ptr<std::vector<Txn*>> txns = std::make_unique<std::vector<Txn*>>();
   Txn test_txn_1(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));

  Txn test_txn_2(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 3, 4})));

  txns->push_back(&test_txn_1);
  txns->push_back(&test_txn_2);

  // create a schedule and make sure that it has been put together
  // correctly.
  //BatchSchedule& bs = BatchSchedule::build_batch_schedule(std::move(txns));
  // TODO: 
  //    These checks are nearly identical to those from above. Delete repeting
  //    chunk and abstract that.
  END;
}

int main(int argc, char** argv) {
  BatchScheduleInsert();
  return 0;
}
