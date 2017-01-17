#ifndef _LOCKQUEUE_TEST_UTILS_H_
#define _LOCKQUEUE_TEST_UTILS_H_

#include "schedule/lockstage_test_utils.h"
#include <memory>

// NOTE: These functions are not a part of LockQueue class 
// because they are not thread safe and should only ever be used 
// within single threaded tests.
//
// Two Lockqueues are considered equal if all elt within the queue
// are equivalent in the sense defined within lockstage_test_utils.h
bool operator==(const LockQueue& lq1, const LockQueue& lq2) {
  std::shared_ptr<LockStage> us, them;
  us = lq1.getCurrent();
  them = lq2.getCurrent();

  // any one of the two is nullptr
  while (us != nullptr || them != nullptr) {
    // only one of the two is nullptr
    if ((us == nullptr && them != nullptr) ||
        (us != nullptr && them == nullptr)) {
      return false;
    }

    if ((*us) != (*them)) {
      return false;
    }

    us = us->get_next_request();
    them = them->get_next_request();
  }

  return true;
}

bool operator!=(LockQueue& lq1, LockQueue& lq2) {
  return !(operator==(lq1, lq2));
}

#endif
