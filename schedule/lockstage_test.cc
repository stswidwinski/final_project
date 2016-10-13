#include "schedule/lockstage.h"
#include "utils/testing.h"
#include "txns/txn.h"

Txn test_txn(
  0,
  std::shared_ptr<std::set<int>>(new std::set<int>({1, 2, 3})),
  std::shared_ptr<std::set<int>>(new std::set<int>({4, 5, 6})));

TEST(AddToStageTestsShared) {
  LockStage ls;
  EXPECT_TRUE(ls.add_to_stage(&test_txn, LockType::shared));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(&test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());
  EXPECT_TRUE(ls.add_to_stage(&test_txn, LockType::shared));
  EXPECT_TRUE(LockType::shared == ls.get_lock_type());

  END;
}

TEST(AddToStageTestsExclusive) {
  LockStage ls;
  EXPECT_TRUE(ls.add_to_stage(&test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(&test_txn, LockType::exclusive));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());
  EXPECT_FALSE(ls.add_to_stage(&test_txn, LockType::shared));
  EXPECT_TRUE(LockType::exclusive == ls.get_lock_type());

  END;
}

TEST(DecrementHoldersTest) {
  LockStage ls;
  for (int i = 0; i < 10; i ++)
    ls.add_to_stage(&test_txn, LockType::shared);

  EXPECT_EQ(10, ls.get_current_holders());

  for (int i = 0; i < 10; i ++) 
    EXPECT_EQ(9-i, ls.decrement_holders());

  EXPECT_EQ(0, ls.get_current_holders());
  END;
}

int main(int argc, char** argv) {
  AddToStageTestsShared();
  AddToStageTestsExclusive();
  DecrementHoldersTest();
}
