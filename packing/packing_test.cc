#include "containers/array_cont.h"
#include "packing/packing.h"
#include "txns/txn.h"
#include "utils/testing.h"

#include <memory>
#include <unordered_set>
#include <vector>

void add_txn_to_vector(std::vector<Txn>* v, std::unordered_set<int> s, unsigned int t_id) {
  Txn t(t_id);
  t.add_to_write_set(s);
  v->push_back(t);
};

std::unordered_set<unsigned int> collect_ids(const std::vector<Txn>& t) {
  std::unordered_set<unsigned int> result;
  for (auto const elt : t) 
    result.insert(elt.get_id());

  return result;
}

template <class A>
inline bool set_contains(std::unordered_set<A> s, A elt) {
  return s.find(elt) != s.end();
}

TEST(SimplePackingTest) {
  // TODO:
  //    Add ID to transactions
  
  // TEST 1   
  // 1 <- T1 <- T2
  // 2 <- T3
  // 3 <- T3 <- T2
  // 4 <- T2
  // Correct packing would be T1, T3
  
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  add_txn_to_vector(v.get(), {1}, 1);
  add_txn_to_vector(v.get(), {1, 3, 4}, 2);
  add_txn_to_vector(v.get(), {2, 3}, 3);

  ArrayContainer c(std::move(v));
  std::vector<Txn> packing = get_packing(&c);
  // TODO: 
  //    After I add IDs, this should just test for IDs.
  EXPECT_EQ(2, packing.size());
  auto ids = collect_ids(packing);
  EXPECT_TRUE(set_contains(ids, 1u));
  EXPECT_TRUE(set_contains(ids, 3u));

  c.sort_remaining();
  packing = get_packing(&c);
  EXPECT_EQ(1, packing.size());
  EXPECT_EQ(3, packing[0].get_write_set_handle()->size());

  END;
}

TEST(SimplePackingTest2) {
  // TEST 2
  // 1 <- T1 <- T2
  // 2 <- T2 <- T3
  // 3 <- T3 <- T4
  // Correct packing would be T1, T4
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  add_txn_to_vector(v.get(), {1}, 1);
  add_txn_to_vector(v.get(), {1, 2}, 2);
  add_txn_to_vector(v.get(), {2, 3}, 3);
  add_txn_to_vector(v.get(), {3}, 4);

  ArrayContainer c(std::move(v));
  std::vector<Txn> packing = get_packing(&c);
  EXPECT_EQ(2, packing.size());
  auto ids = collect_ids(packing);
  EXPECT_TRUE(set_contains(ids, 1u));
  EXPECT_TRUE(set_contains(ids, 4u));

  END;
}

// TODO:
//    Write tests with shared locks.

int main () {
  SimplePackingTest();
  SimplePackingTest2();
}


