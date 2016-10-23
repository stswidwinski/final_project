#include "containers/array_cont.h"
#include "packing/packing.h"
#include "txns/txn.h"
#include "utils/testing.h"

#include <memory>
#include <unordered_set>
#include <vector>

void add_txn_to_vector(
    std::vector<Txn>* v, 
    unsigned int t_id,
    std::unordered_set<int> write_set, 
    std::unordered_set<int> read_set = std::unordered_set<int>()) {
  Txn t(t_id);
  t.add_to_write_set(write_set);
  t.add_to_read_set(read_set);
  v->push_back(t);
};

std::unordered_set<unsigned int> collect_ids(const std::vector<Txn>& t) {
  std::unordered_set<unsigned int> result;
  for (auto const elt : t) 
    result.insert(elt.get_id());

  return result;
}

TEST(SimplePackingTest) {
  // TEST 1   
  // 1 <- T1 <- T2
  // 2 <- T3
  // 3 <- T3 <- T2
  // 4 <- T2
  // Correct packing would be T1, T3
  
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  add_txn_to_vector(v.get(), 1, {1});
  add_txn_to_vector(v.get(), 2, {1, 3, 4});
  add_txn_to_vector(v.get(), 3, {2, 3});

  ArrayContainer c(std::move(v));
  std::vector<Txn> packing = get_packing(&c);
  auto ids = collect_ids(packing);
  EXPECT_TRUE(
      std::unordered_set<unsigned int>({1, 3}) == ids);
  EXPECT_EQ(1, c.get_remaining_count());

  c.sort_remaining();
  packing = get_packing(&c);
  EXPECT_EQ(1, packing.size());
  EXPECT_EQ(2, packing[0].get_id());
  EXPECT_EQ(0, c.get_remaining_count());

  END;
}

TEST(SimplePackingTest2) {
  // TEST 2
  // 1 <- T1 <- T2
  // 2 <- T2 <- T3
  // 3 <- T3 <- T4
  // Correct packing would be T1, T4
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  add_txn_to_vector(v.get(), 1, {1});
  add_txn_to_vector(v.get(), 2, {1, 2});
  add_txn_to_vector(v.get(), 3, {2, 3});
  add_txn_to_vector(v.get(), 4, {3});

  ArrayContainer c(std::move(v));
  std::vector<Txn> packing = get_packing(&c);
  auto ids = collect_ids(packing);
  EXPECT_TRUE(
      std::unordered_set<unsigned int>({1, 4}) == ids);
  EXPECT_EQ(2, c.get_remaining_count());

  END;
}

TEST(SimplePackingSharedLockTest1) {
  // TEST 3
  // 1 <- T1s <- T2s <- T3s
  // 2 <- T1 <- T3s
  // 3 <- T2 <- T3s
  // Correct packing: T1, T2 and T3 afterwards
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  add_txn_to_vector(v.get(), 1, {2}, {1});
  add_txn_to_vector(v.get(), 2, {3}, {1});
  add_txn_to_vector(v.get(), 3, {}, {1, 2, 3});

  ArrayContainer c(std::move(v));
  std::vector<Txn> packing = get_packing(&c);
  auto ids = collect_ids(packing);
  EXPECT_TRUE(
      std::unordered_set<unsigned int>({1, 2}) == ids);
  EXPECT_EQ(1, c.get_remaining_count());

  c.sort_remaining();
  packing = get_packing(&c);
  EXPECT_EQ(1, packing.size());
  EXPECT_EQ(3, packing[0].get_id());
  EXPECT_EQ(0, c.get_remaining_count());

  END;
}

// TODO:
//    Write tests with shared locks.

int main () {
  SimplePackingTest();
  SimplePackingTest2();
  SimplePackingSharedLockTest1();
}


