#ifndef _TEST_LOCK_TABLE_H_
#define _TEST_LOCK_TABLE_H_

#include "schedule/locktable.h"

class TestLockTable : public LockTable {
  friend void InsertLockRequestTest();
  friend void FinalizeLockRequestTest(); 
  friend void MergeTest();
};

#endif // _TEST_LOCK_TABLE_H_
