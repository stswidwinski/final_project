#include "schedule/schedule.h"
#include "utils/testing.h"

TEST(ScheduleMergingIntoEmptyTest) {
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
 
  Schedule s;
  s.merge_batch_schedule_in(bs);
  
  EXPECT_EQ(
    1,
    s.lock_table.ready_queue.ready_queue.size());
  EXPECT_EQ(
    &test_txn_1,
    s.get_txn_to_execute());
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());

  s.finalize_txn(&test_txn_1);
  EXPECT_EQ(
    &test_txn_2,
    s.get_txn_to_execute());
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());

  END;
}

TEST(ScheduleMergingIntoExistingTest) {
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
  
  Schedule s;
  s.merge_batch_schedule_in(bs);

  // Create a second batch schedule with conflicting 
  // and non-conflicting elements.
  BatchSchedule bs1;
  // should be ready inmediately.
  Txn test_txn_3(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({4})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2})));
  // should not be ready inmediately.
  Txn test_txn_4(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1})),
    std::shared_ptr<std::set<int>>(new std::set<int>({2, 3})));
 
  bs1.add_txn(&test_txn_3);
  bs1.add_txn(&test_txn_4);

  s.merge_batch_schedule_in(bs1);

  EXPECT_EQ(
    2,
    s.lock_table.ready_queue.ready_queue.size());
  EXPECT_EQ(
    &test_txn_1,
    s.get_txn_to_execute());
  EXPECT_EQ(
    &test_txn_3,
    s.get_txn_to_execute());
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());

  s.finalize_txn(&test_txn_1);
  EXPECT_EQ(
    1,
    s.lock_table.ready_queue.ready_queue.size());
  EXPECT_EQ(
    &test_txn_2,
    s.get_txn_to_execute());
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());

  s.finalize_txn(&test_txn_3);
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());

  s.finalize_txn(&test_txn_2);
  EXPECT_EQ(
    1,
    s.lock_table.ready_queue.ready_queue.size());
  EXPECT_EQ(
    &test_txn_4,
    s.get_txn_to_execute());
  EXPECT_EQ(
    0,
    s.lock_table.ready_queue.ready_queue.size());
  
  END;
}

int main(int argc, char** argv) {
  ScheduleMergingIntoEmptyTest();
  ScheduleMergingIntoExistingTest();
  return 0;
}