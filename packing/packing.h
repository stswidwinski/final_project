#include "containers/array_cont.h"
#include "txns/txn.h"

#include <vector>
#include <unordered_set>

typedef std::vector<std::unique_ptr<Txn>> txn_vector; 
typedef std::unique_ptr<txn_vector> txn_vector_pt;

bool txn_conflicts(Txn* t, std::unordered_set<int>* ex, std::unordered_set<int>* sh) {
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

txn_vector get_packing(Container* c) {
  std::unordered_set<int> held_exclusive_locks;
  std::unordered_set<int> held_shared_locks;
  txn_vector txn_in_packing;
  Txn* next_txn;
  std::unique_ptr<Txn> txn;

  auto merge_sets = [](std::unordered_set<int>* mergeTo, std::set<int>* mergeFrom) {
    for (auto it = mergeFrom->begin(); it != mergeFrom->end(); it ++) {
      mergeTo->insert(*it);
    }
  };

  while ( (next_txn = c->peak_curr_min_elt()) != nullptr) {
    if (!txn_conflicts(next_txn, &held_exclusive_locks, &held_shared_locks)) {
      // add the txn to packing, record the locks.
      merge_sets(&held_exclusive_locks, next_txn->get_write_set_handle().get());
      merge_sets(&held_shared_locks, next_txn->get_read_set_handle().get());
      // move the ownership
      txn = c->take_curr_min_elt();
      txn_in_packing.push_back(std::move(txn));
      continue;
    }

    c->advance_to_next_min();
  } 

  return txn_in_packing;
}
