#include "lock/lock.h"
#include "schedule/batchschedule.h"
#include "utils/testing.h"
#include "schedule/locktable.h"

typedef std::shared_ptr<Txn> txn_pt;

TEST(BatchScheduleInsert) {
  BatchSchedule bs;

  txn_pt test_txn_1 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));

  txn_pt test_txn_2 = std::make_shared<Txn>(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 3})));

  bs.add_txn(test_txn_1);
  bs.add_txn(test_txn_2);
  LockTable& lt = bs.lock_table;
  // The lock table for lock # 1 set properly
  LockStage expected_newest_1 = LockStage(
    std::unordered_set<txn_pt>{test_txn_2},
    LockType::exclusive);
  LockStage expected_current_1 = LockStage(
    std::unordered_set<txn_pt>{test_txn_1},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_1));
  LockQueue expected_locktable_1 = LockQueue(
    std::make_shared<LockStage>(expected_current_1),
    std::make_shared<LockStage>(expected_newest_1));

  EXPECT_TRUE(*(lt.lock_table.find(1)->second) == expected_locktable_1);
  
  // The lock table for lock # 2 set properly
  LockStage expected_new_cur_2 = LockStage(
    std::unordered_set<txn_pt>{test_txn_1, test_txn_2},
    LockType::shared);
  LockQueue expected_queue_2 = LockQueue(
    std::make_shared<LockStage>(expected_new_cur_2));

  EXPECT_TRUE(*(lt.lock_table.find(2)->second) == expected_queue_2);

  // The lock table for lock # 3 set properly.
  LockStage expected_newest_3 = LockStage(
    std::unordered_set<txn_pt>{test_txn_2},
    LockType::shared);
  LockStage expected_current_3 = LockStage(
    std::unordered_set<txn_pt>{test_txn_1},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_3));
  LockQueue expected_queue_3 = LockQueue(
    std::make_shared<LockStage>(expected_current_3),
    std::make_shared<LockStage>(expected_newest_3));

  EXPECT_TRUE(*(lt.lock_table.find(3)->second) == expected_queue_3);
  
  END; 
}

TEST(BatchScheduleCreationTest1) {
  // The logical lock-table view of the transactions:
  // 1 <- T1 <- T2
  // 2 <- T1s <- T2s
  // 3 <- T1 <- T2s
  // 4 <- T1s <- T2
  // 5 <- T2s
  // The batch schedule must correspond to it. This schedule
  // has all the combinations of txns possible. Well, almost
  // since all the packings are just 1 txn each. The next tests checks
  // for handling that. 
  std::unique_ptr<std::vector<std::unique_ptr<Txn>>> txns = 
    std::make_unique<std::vector<std::unique_ptr<Txn>>>();
  txns->push_back(std::make_unique<Txn>(
    1,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 4}))));
  txns->push_back(std::make_unique<Txn>(
    2,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 4})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 3, 5}))));

  // create a schedule and make sure that it has been put together
  // correctly.
  std::unique_ptr<BatchSchedule> bs = BatchSchedule::build_batch_schedule(std::move(txns));
  LockTable& lt = bs->lock_table;

  // The lock table for lock #1
  // Since the memory used by txns vector goes away after move, we must obtain the addresses
  // of txns within maps by hand using the debug variable.
  LockStage expected_newest_1 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::exclusive);
  LockStage expected_current_1 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(1)->second},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_1));
  LockQueue expected_queue_1 = LockQueue(
    std::make_shared<LockStage>(expected_current_1),
    std::make_shared<LockStage>(expected_newest_1));
  EXPECT_TRUE(*(lt.lock_table.find(1)->second) == expected_queue_1);

  // The lock table for lock #2
  LockStage expected_current_2 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{
      bs->txns.find(1)->second, bs->txns.find(2)->second},
    LockType::shared);
  LockQueue expected_queue_2 = LockQueue(
    std::make_shared<LockStage>(expected_current_2));
  EXPECT_TRUE(*(lt.lock_table.find(2)->second) == expected_queue_2);

  // The lock table for lock #3
  LockStage expected_newest_3 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::shared);
  LockStage expected_current_3 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(1)->second},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_3));
  LockQueue expected_queue_3 = LockQueue(
    std::make_shared<LockStage>(expected_current_3),
    std::make_shared<LockStage>(expected_newest_3));
  EXPECT_TRUE(*(lt.lock_table.find(3)->second) == expected_queue_3);

  // The lock table for lock #4
  LockStage expected_newest_4 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::exclusive);
  LockStage expected_current_4 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(1)->second},
    LockType::shared,
    std::make_shared<LockStage>(expected_newest_4));
  LockQueue expected_queue_4 = LockQueue(
    std::make_shared<LockStage>(expected_current_4),
    std::make_shared<LockStage>(expected_newest_4));
  EXPECT_TRUE(*(lt.lock_table.find(4)->second) == expected_queue_4);

  // The lock table for lock #5
  LockStage expected_current_5 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::shared);
  LockQueue expected_queue_5 = LockQueue(
    std::make_shared<LockStage>(expected_current_5));
  EXPECT_TRUE(*(lt.lock_table.find(5)->second) == expected_queue_5);

  END;
}

TEST(BatchScheduleCreationTest2) {
  // The logical lock-table view of the transactions:
  // 1 <- T1s <- T2 <- T3s
  // 2 <- T2
  // 3 <- T4
  // The batch schedule must correspond to it. Tests mostly
  // for multiple txns in a single packing. 
  std::unique_ptr<std::vector<std::unique_ptr<Txn>>> txns = 
    std::make_unique<std::vector<std::unique_ptr<Txn>>>();
  txns->push_back(std::make_unique<Txn>(
    1,
    std::shared_ptr<std::set<int>>(new std::set<int>({})),
    std::shared_ptr<std::set<int>>(new std::set<int>({1}))));
  txns->push_back(std::make_unique<Txn>(
    2,
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({}))));
  txns->push_back(std::make_unique<Txn>(
    3,
    std::shared_ptr<std::set<int>>(new std::set<int>({})),
    std::shared_ptr<std::set<int>>(new std::set<int>({1}))));
  txns->push_back(std::make_unique<Txn>(
    4,
    std::shared_ptr<std::set<int>>(new std::set<int>({3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({}))));

  // create a schedule and make sure that it has been put together
  // correctly.
  std::unique_ptr<BatchSchedule> bs = BatchSchedule::build_batch_schedule(std::move(txns));
  LockTable& lt = bs->lock_table;
  // The lock table for lock #1
  LockStage expected_newest_1 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::exclusive);
  LockStage expected_current_1 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{
      bs->txns.find(1)->second, bs->txns.find(3)->second},
    LockType::shared,
    std::make_shared<LockStage>(expected_newest_1));
  LockQueue expected_queue_1 = LockQueue(
    std::make_shared<LockStage>(expected_current_1),
    std::make_shared<LockStage>(expected_newest_1));
  EXPECT_TRUE(*(lt.lock_table.find(1)->second) == expected_queue_1);

  // The lock table for lock #2
  LockStage expected_current_2 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(2)->second},
    LockType::exclusive);
  LockQueue expected_queue_2 = LockQueue(
    std::make_shared<LockStage>(expected_current_2));
  EXPECT_TRUE(*(lt.lock_table.find(2)->second) == expected_queue_2);

  // The lock table for lock #3
  LockStage expected_current_3 = LockStage(
    std::unordered_set<std::shared_ptr<Txn>>{bs->txns.find(4)->second},
    LockType::exclusive);
  LockQueue expected_queue_3 = LockQueue(
    std::make_shared<LockStage>(expected_current_3));
  EXPECT_TRUE(*(lt.lock_table.find(3)->second) == expected_queue_3);

  END;
}

int main(int argc, char** argv) {
  BatchScheduleInsert();
  BatchScheduleCreationTest1();
  BatchScheduleCreationTest2();
  return 0;
}
