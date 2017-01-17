#ifndef _LOCKSTAGE_TEST_UTILS_H_
#define _LOCKSTAGE_TEST_UTILS_H_

// define equality of lockstages as their equivalence (the same number of holders,
// the same lock type and the same of requesters). 
//
// NOTE: This is not defined as a part of the class because it's only used 
//      within tests and it is NOT thread-safe!
bool operator==(const LockStage& ls1, const LockStage& ls2) {
  return (
      ls1.holders == ls2.holders &&
      ls1.type == ls2.type &&
      ls1.requesters == ls2.requesters);
}

bool operator!=(const LockStage& ls1, const LockStage& ls2) {
  return !(operator==(ls1, ls2));
}

#endif // _LOCKSTAGE_TEST_UTILS_H_
