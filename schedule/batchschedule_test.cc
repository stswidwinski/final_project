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

TEST(BatchScheduleCreation) {
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

  // create a schedule and make sure that it has been put together
  // correctly.
  std::unique_ptr<BatchSchedule> bs = BatchSchedule::build_batch_schedule(std::move(txns));
  LockTable& lt = bs->get_lock_table();

  // The lock table for lock #1
  // Since the memory used by txns vector goes away after move, we must obtain the addresses
  // of txns within maps by hand using the debug variable.
  LockStage expected_newest_1 = LockStage(
    std::unordered_set<Txn*>{bs->container->get_txn_by_id(2)},
    LockType::exclusive);
  LockStage expected_current_1 = LockStage(
    std::unordered_set<Txn*>{bs->container->get_txn_by_id(1)},
    LockType::exclusive,
    std::make_shared<LockStage>(expected_newest_1));
  LockQueue expected_queue_1 = LockQueue(
    std::make_shared<LockStage>(expected_current_1),
    std::make_shared<LockStage>(expected_newest_1));
  EXPECT_TRUE(*(lt.lock_table.find(1)->second) == expected_queue_1);

  // The lock table for lock #2
  LockStage expected_current_2 = LockStage(
    std::unordered_set<Txn*>{
      bs->container->get_txn_by_id(1), bs->container->get_txn_by_id(2)},
    LockType::shared);
  LockQueue expected_queue_2 = LockQueue(
    std::make_shared<LockStage>(expected_current_1));
  EXPECT_TRUE(*(lt.lock_table.find(2)->second) == expected_queue_2);
//
 // TODO: 
  //    These checks are nearly identical to those from above. Delete repeting
  //    chunk and abstract that.
  END;
}

int main(int argc, char** argv) {
  BatchScheduleInsert();
  BatchScheduleCreationTest1();
  return 0;
}
