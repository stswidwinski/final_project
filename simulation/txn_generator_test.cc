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

int main (int argc, char** argv) {
  RandWithoutRepeatsTest();

  return 0;
}
