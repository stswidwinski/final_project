#include "containers/array_cont.h"
#include "txns/txn.h"

#include <vector>
#include <unordered_set>

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
  
  auto merge_sets = [](std::unordered_set<int>* mergeTo, std::set<int>* mergeFrom) {
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
