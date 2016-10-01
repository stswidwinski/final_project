#include "containers/array_cont.h"
#include <algorithm>

Txn* ArrayContainer::get_next_min_elt() {
  if (current_min_index >= txn_arr->size() ||
      current_min_index < current_barrier_index)
    return nullptr;

  current_min_index ++; 
  return &((*txn_arr)[current_min_index - 1]);
}

void ArrayContainer::remove_former_min() {
  if (current_min_index < 1)
    return;

  // swap the former min with current barrier index.
  Txn tmp = std::move((*txn_arr)[current_min_index - 1]);
  (*txn_arr)[current_min_index - 1] = std::move((*txn_arr)[current_barrier_index]);
  (*txn_arr)[current_barrier_index] = std::move(tmp);
  current_barrier_index ++;
}

void ArrayContainer::sort_remaining() {
  std::sort(txn_arr->begin() + current_barrier_index, txn_arr->end());
  current_min_index = current_barrier_index;
}
