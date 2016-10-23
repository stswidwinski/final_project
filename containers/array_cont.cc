#include "containers/array_cont.h"
#include <algorithm>

Txn* ArrayContainer::peak_curr_min_elt() {
  if (current_min_index >= txn_arr->size() ||
      current_min_index < current_barrier_index)
    return nullptr;

  return ((*txn_arr)[current_min_index]).get();
}

std::unique_ptr<Txn> ArrayContainer::take_curr_min_elt() {
  if (current_min_index >= txn_arr->size() ||
      current_min_index < current_barrier_index)
    return nullptr;

  // swap the current min with the current barrier index one
  std::unique_ptr<Txn> min = std::move((*txn_arr)[current_min_index]);
  (*txn_arr)[current_min_index] = std::move((*txn_arr)[current_barrier_index]);

  current_min_index ++;
  current_barrier_index ++;
  return min; 
}

void ArrayContainer::advance_to_next_min() {
  current_min_index ++;
}

void ArrayContainer::sort_remaining() {
  // sort by what is being pointed to.
  std::sort(
      txn_arr->begin() + current_barrier_index, 
      txn_arr->end(),
      [](std::unique_ptr<Txn> const& a, std::unique_ptr<Txn> const& b) {
        return *a < *b;
      });
  current_min_index = current_barrier_index;
}

unsigned int ArrayContainer::get_remaining_count() {
  return txn_arr->size() - current_barrier_index;
}
