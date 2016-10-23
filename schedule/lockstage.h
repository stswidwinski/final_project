#ifndef _LOCK_STAGE_H_
#define _LOCK_STAGE_H_

#include "lock/lock.h"
#include "txns/txn.h"

#include <memory>

class LockStage {
private:
  int holders;
  std::shared_ptr<LockStage> next_request;
  LockType type;
  std::unordered_set<std::shared_ptr<Txn>> requesters;

public:
  // TODO: 
  //    Phase out holders.
  LockStage():
    holders(0),
    next_request(nullptr),
    type(LockType::shared),
    requesters({})
   {};

  LockStage(
      std::unordered_set<std::shared_ptr<Txn>> reqers, 
      LockType t, 
      std::shared_ptr<LockStage> ns = nullptr):
    holders(reqers.size()),
    next_request(ns),
    type(t),
    requesters(reqers)
   {};

  // Returns true if successful and false otherwise.
  bool add_to_stage(std::shared_ptr<Txn> txn, LockType req_type);
  // Returns the new value of holders after decrementation.
  int decrement_holders();
  void set_next_stage(std::shared_ptr<LockStage> ls);

  // getters
  int get_current_holders();
  std::shared_ptr<LockStage> get_next_request();
  std::unordered_set<std::shared_ptr<Txn>>& get_requesters();
  LockType get_lock_type();

  // Two LockStages are equal if they holders, type and requesters
  // are the same. The next_request may be different,
  //
  // NOTE:
  //    USE IN TESTS ONLY.
  bool operator==(const LockStage& ls) const;
  bool operator!=(const LockStage& ls) const;

  friend void BatchScheduleInsert();
};

#endif // _LOCK_STAGE_H_
