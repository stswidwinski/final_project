#include "schedule/lockstage.h"
#include "utils/testing.h"
#include "txns/txn.h"
#include "schedule/lockstage_test_utils.h"

std::shared_ptr<Txn> test_txn = std::make_shared<Txn>(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1, 2, 3})),
  std::shared_ptr<std::set<int>>(new std::set<int>({4, 5, 6})));

std::shared_ptr<Txn> test_txn2 = std::make_shared<Txn>(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1, 2, 3})),
  std::shared_ptr<std::set<int>>(new std::set<int>({4, 5, 6})));

TEST(AddToStageTestsShared) {
  LockStage ls;
  EXPECT_TRUE(ls.add_to_stage(test_txn, LockType::shared));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());
  EXPECT_TRUE(ls.add_to_stage(test_txn, LockType::shared));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());

  END;
}

TEST(AddToStageTestsExclusive) {
  LockStage ls;
  EXPECT_TRUE(ls.add_to_stage(test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(test_txn, LockType::shared));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());

  END;
}

TEST(DecrementHoldersTest) {
  LockStage ls;
  for (int i = 0; i < 10; i ++)
    ls.add_to_stage(test_txn, LockType::shared);

  EXPECT_EQ(10, ls.get_current_holders());

  for (int i = 0; i < 10; i ++) 
    EXPECT_EQ(9-i, ls.decrement_holders());

  EXPECT_EQ(0, ls.get_current_holders());
  END;
}

TEST(EqualityTesting) {
  // exclusive lock stage equality
  LockStage ls1;
  LockStage ls2;
  EXPECT_TRUE(ls1 == ls2);
  ls1.add_to_stage(test_txn, LockType::exclusive);
  EXPECT_FALSE(ls1 == ls2);
  ls2.add_to_stage(test_txn, LockType::exclusive);
  EXPECT_TRUE(ls1 == ls2);
  // changing the next stage should not change anything
  ls1.set_next_stage(std::make_shared<LockStage>(ls2));
  EXPECT_TRUE(ls1 == ls2);

  // exclusive lock stage inequality
  LockStage ls3;
  LockStage ls4;
  ls3.add_to_stage(test_txn, LockType::exclusive);
  EXPECT_FALSE(ls3 == ls4);
  ls4.add_to_stage(test_txn2, LockType::exclusive);
  EXPECT_FALSE(ls3 == ls4);

  // shared lock stage equality
  LockStage ls5;
  LockStage ls6;
  ls5.add_to_stage(test_txn, LockType::shared);
  EXPECT_FALSE(ls5 == ls6);
  ls6.add_to_stage(test_txn, LockType::shared);
  EXPECT_TRUE(ls5 == ls6);
  // changing the next stage should not change anything
  ls5.set_next_stage(std::make_shared<LockStage>(ls6));
  EXPECT_TRUE(ls5 == ls6);

  // shared lock stage inequality
  LockStage ls7;
  LockStage ls8;
  ls7.add_to_stage(test_txn, LockType::shared);
  EXPECT_FALSE(ls7 == ls8);
  ls8.add_to_stage(test_txn2, LockType::shared);
  EXPECT_FALSE(ls7 == ls8);
  ls7.add_to_stage(test_txn2, LockType::shared);
  EXPECT_FALSE(ls7 == ls8);
  ls8.add_to_stage(test_txn, LockType::shared);
  EXPECT_TRUE(ls7 == ls8);
  // changing the next stage should not change anything
  ls7.set_next_stage(std::make_shared<LockStage>(ls8));
  EXPECT_TRUE(ls7 == ls8);

  // shared versus exclusive stage inequality
  LockStage ls9;
  LockStage ls10;
  ls9.add_to_stage(test_txn, LockType::shared);
  ls10.add_to_stage(test_txn, LockType::exclusive);
  EXPECT_FALSE(ls9 == ls10);

  END;
}

int main(int argc, char** argv) {
  AddToStageTestsShared();
  AddToStageTestsExclusive();
  DecrementHoldersTest();
  EqualityTesting();
}
