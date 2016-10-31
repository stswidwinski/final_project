#include "simulation/txn_map.h"
#include "utils/testing.h"
#include "txns/txn.h"
#include "simulation/txn_sim_wrapper.h"

std::vector<TxnWrapper> get_sample_test_load() {
  std::vector<TxnWrapper> res;

  res.push_back(TxnWrapper(Txn(0), 1, 1));
  res.push_back(TxnWrapper(Txn(1), 2, 2));
  res.push_back(TxnWrapper(Txn(2), 3, 3));
  res.push_back(TxnWrapper(Txn(3), 4, 4));

  return res;
}

TEST(AvgCompletionTimeTest) {
  TxnMap m(get_sample_test_load());

  // set completion times
  for (unsigned int i = 0; i < 4; i ++) {
    m.set_start_time(i, i + 1);
  }

  EXPECT_EQ(2.5 , m.get_average_completion_time());
  END;
}

TEST(GetTxnsBtwTest) {
  TxnMap m(get_sample_test_load());
  std::vector<std::unique_ptr<Txn>> res = m.get_txns_between(2,3);
  EXPECT_EQ(2, res.size());
  for (unsigned int i = 1; i < 3; i++) 
    EXPECT_EQ(i, res[i-1]->get_id());

  std::vector<TxnWrapper> load2;
  load2.push_back(TxnWrapper(Txn(0), 2, 1));
  load2.push_back(TxnWrapper(Txn(1), 2, 2));
  TxnMap m2(load2);
  res = m2.get_txns_between(2, 2);
  EXPECT_EQ(2, res.size());
  for (unsigned int i = 0; i < 2; i++)
    EXPECT_EQ(i, res[i]->get_id());

  END;
}

int main(int argc, char** argv) {
  AvgCompletionTimeTest();
  GetTxnsBtwTest();
  return 0;
}
