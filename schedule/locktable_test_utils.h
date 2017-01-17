#ifndef _LOCKTABLE_TEST_UTILS_H_
#define _LOCKTABLE_TEST_UTILS_H_

#include "schedule/lockqueue_test_utils.h"

// Two LockTables are equal if 
//    1) they contain the same elements
//      and the lockqueues are equal for every elements
//    2) the ready queue contains the same elements in the
//      same order.
//
// NOTE:
//    THIS FUNCTION IS NOT THREAD SAFE. USE IN SINGLE-THREAD
//    TESTS ONLY.
bool operator==(const LockTable& lt1, const LockTable& lt2) {
  // we must have the same number of elt in the lock tables.
  if (lt1.lock_table.size() != lt2.lock_table.size())
    return false;

  // the ready queues must have the same size.
  if (lt1.ready_queue.ready_queue.size() != lt2.ready_queue.ready_queue.size())
    return false;

  // every element in lock_table must be present in lts lock table 
  // and map to the same elt.
  for (auto entry : lt1.lock_table) {
    if (lt2.lock_table.find(entry.first) == lt2.lock_table.end())
      return false;

    if ((*entry.second) != (*lt2.lock_table.find(entry.first)->second))
      return false;
  }

  // every elt in ready queue must be equivalent.
  auto usIt = lt1.ready_queue.ready_queue.begin();
  auto theyIt = lt2.ready_queue.ready_queue.begin();
  for (; usIt != lt1.ready_queue.ready_queue.end(); usIt++, theyIt++) {
    if (*usIt != *theyIt)
      return false;
  }

  return true;
}

bool operator!=(const LockTable& lt1, const LockTable& lt2) {
  return !(operator==(lt1, lt2));
}

#endif //_LOCKTABLE_TEST_UTILS_H_
