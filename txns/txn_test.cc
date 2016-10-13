#include "txns/txn.h"
#include "utils/testing.h"

#include <iostream>

TEST(TxnLocksGrantedTest) {
  Txn t(
    0,
    std::shared_ptr<std::set<int>>(new std::set<int>({1, 2, 3})),
    std::shared_ptr<std::set<int>>(new std::set<int>({4, 5, 6})));

  for (unsigned int i = 0; i < 6; i ++) {
    if (i == 5) {
      EXPECT_EQ(true, t.lock_granted());
      break;
    }
    EXPECT_EQ(false, t.lock_granted());
  }
  END;
}

int main(int argc, char** argv) {
  TxnLocksGrantedTest();
}
