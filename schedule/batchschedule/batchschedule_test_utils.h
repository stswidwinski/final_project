#ifndef _BATCHSCHEDULE_TEST_UTILS_H_
#define _BATCHSCHEDULE_TEST_UTILS_H_

#include "schedule/locktable/locktable_test_utils.h"

bool operator==(
    const BatchSchedule& bs1, 
    const BatchSchedule& bs2) {
  return bs1.lock_table == bs2.lock_table;
}

#endif // _BATCHSCHEDULE_TEST_UTILS_H_
