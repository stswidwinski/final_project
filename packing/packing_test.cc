#include "containers/array_cont.h"
#include "txns/txn.h"

#include <unordered_set>
#include <memory>
#include <iostream>

std::vector<Txn> get_packing(Container*);
bool test_get_packing();

int main () {
  // TODO:
  //    Convert this to google unit tests.
  test_get_packing();
}

bool test_get_packing() {
  // TODO:
  //    Add ID to transactions
  
  // TEST 1   
  // 1 <- T1 <- T2
  // 2 <- T3
  // 3 <- T3 <- T2
  // 4 <- T2
  // Correct packing would be T1, T3
  
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  auto add_txn_to_vector = [] (std::vector<Txn>* v, std::unordered_set<int> s) {
    Txn t;
    t.add_to_write_set(s);
    v->push_back(t);
  };
  add_txn_to_vector(v.get(), {1});
  add_txn_to_vector(v.get(), {1, 3, 4});
  add_txn_to_vector(v.get(), {2, 3});

  ArrayContainer c(std::move(v));
  std::vector<Txn> res = get_packing(&c);
  // TODO: 
  //    After I add IDs, this should just test for IDs.
  if (res.size() != 2) {
    std::cout << "Test failed. Size of packing wrong.\n";
    return 1;
  }

  for (const auto& t : res) {
    auto w = t.get_write_set_handle();
    if (w->size() > 2) {
      std::cout << "Test failed. Packing contains T3!\n";
      return 1;
    }
  }

  c.sort_remaining();
  res = get_packing(&c);
  if (res.size() != 1 ||
      res[0].get_write_set_handle()->size() != 3) {
    std::cout << "Multiple use of packing fails.\n";
    return 1;
  }
  
  std::cout << "Test passed\n";
  return 0;
  }

bool txn_conflicts(Txn* t, std::unordered_set<int>* ex, std::unordered_set<int>* sh) {
  // TODO: 
  //    Optimize this -- iterate through the smaller of two sets.

  auto t_ex = t->get_write_set_handle();
  auto t_sh = t->get_read_set_handle();
  auto contains = [] (std::unordered_set<int>* s, int elt) {
    return (s->find(elt) != s->end());
  };

  // exclusive locks conflict with all locks.
  for (auto it = t_ex->begin(); it != t_ex->end(); it ++) {
    if (contains(ex, *it) ||
        contains(sh, *it)) {
      return true;
    }
  }

  // shared locks only conflict with exclusive locks.
  for (auto it = t_sh->begin(); it != t_sh->end(); it ++) {
    if (contains(ex, *it)) {
      return true;
    }
  }

  return false;
}

std::vector<Txn> get_packing(Container* c) {
  std::unordered_set<int> held_exclusive_locks;
  std::unordered_set<int> held_shared_locks;
  std::vector<Txn> txn_in_packing;
  Txn* next_txn;
  
  auto merge_sets = [](std::unordered_set<int>* mergeTo, std::unordered_set<int>* mergeFrom) {
    for (auto it = mergeFrom->begin(); it != mergeFrom->end(); it ++) {
      mergeTo->insert(*it);
    }
  };

  while ( (next_txn = c->get_next_min_elt()) != nullptr) {
    if (!txn_conflicts(next_txn, &held_exclusive_locks, &held_shared_locks)) {
      // add the txn to packing, record the locks.
      txn_in_packing.push_back(*next_txn);
      merge_sets(&held_exclusive_locks, next_txn->get_write_set_handle().get());
      merge_sets(&held_shared_locks, next_txn->get_read_set_handle().get());
      // discard the txn in the future
      c->remove_former_min();
    }
  } 

  return txn_in_packing;
}
