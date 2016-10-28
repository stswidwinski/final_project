#include "simulation/txn_generator.h"
#include "utils/testing.h"

// Very simple test that might probabilistically not work very little of the time.
// Sure, that's a bug, but this isn't going to be powering NASA rockets.
TEST(RandWithoutRepeatsTest) {
  // Test that given an array the function returns a pseudo-random
  // permutation of it.
  TxnGenerator g;
  auto result = g.rand_without_repeats(100, 100, 0);
  
  // we definitely should NOT get something in order. Unless we are extremely unlucky
  // but the probability of that happening is literally negligible
  bool inOrder = true;
  for (int i = 0; i < 100; i++) {
    if (result[i] != i) {
      inOrder = false;
      continue;
    }
  }
  EXPECT_FALSE(inOrder);

  // we only want elements within range 0 - 99 AND they must be unique
  std::unordered_set<int> set;
  for (int i = 0; i < 100; i++) {
    EXPECT_TRUE(result[i] >= 0 && result[i] < 100);
    EXPECT_TRUE(set.find(result[i]) == set.end());
    set.insert(result[i]);
  }

  END;
}

TEST(GenUniformTxnsTest) {
  // this only tests that there are no segfaults and other errors like that.
  // The real test for uniformity etc. are done by visual inspection of
  // resulting distributions and the testing is not rigorous in that sense.
  //
  // TL;DR
  // Testing random engines with multiple distributions is mundane and hard.
  TxnGenerator g;
  auto test_txn = [](
      TxnWrapper& tw,
      bool no_write_locks,
      bool no_read_locks) {
    if (no_write_locks) 
      EXPECT_EQ(0, tw.t.get_write_set_handle()->size());
    else
      EXPECT_UNEQ(0, tw.t.get_write_set_handle()->size());

    if (no_read_locks)
      EXPECT_EQ(0, tw.t.get_read_set_handle()->size());
    else
      EXPECT_UNEQ(0, tw.t.get_read_set_handle()->size());

    EXPECT_TRUE(tw.start_time >= 0 && tw.start_time < 10);
    unsigned int locks =
      tw.t.get_read_set_handle()->size() +
      tw.t.get_write_set_handle()->size();
    EXPECT_EQ(tw.exec_duration, locks);
  };

  // no write txns, only uncont locks, 5 txns.
  g.set_time_period(10);
  g.set_txn_num(5);
  g.set_lock_time_mult(1);
  g.set_uncont_lock_info(100, 10, 2);
  g.set_cont_lock_info(0, 0, 0);

  auto txns = g.gen_uniform();
  EXPECT_EQ(5, txns.size());
  for (auto& txnWrap : txns) {
    test_txn(txnWrap, true, false);
  }

  // same test, but with write txns
  g.set_write_txn_perc(1.0);
  txns = g.gen_uniform();
  EXPECT_EQ(5, txns.size());
  for (auto& txnWrap : txns) {
    test_txn(txnWrap, false, true);
  }

  // and the same with a mix of the two
  g.set_write_txn_perc(0.2);
  unsigned int num_write_txns = 0;
  bool is_write_txn = false;
  txns = g.gen_uniform();
  EXPECT_EQ(5, txns.size());
  for (auto& txnWrap : txns) {
    // this might fail with negligible probability
    is_write_txn = (txnWrap.t.get_write_set_handle()->size() != 0);
    num_write_txns += is_write_txn;

    test_txn(txnWrap, !is_write_txn, is_write_txn);
  }
  EXPECT_EQ(1, num_write_txns);

  END;
}

int main (int argc, char** argv) {
  RandWithoutRepeatsTest();
  GenUniformTxnsTest();
  
  return 0;
}
