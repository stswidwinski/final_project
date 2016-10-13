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
  std::unordered_set<Txn*> requesters;

public:
  LockStage():
    holders(0),
    next_request(nullptr),
    type(LockType::shared),
    requesters({})
   {};

  LockStage(std::unordered_set<Txn*> reqers, LockType t):
    holders(0),
    next_request(nullptr),
    type(t),
    requesters(reqers)
   {};

  // Returns true if successful and false otherwise.
  bool add_to_stage(Txn* txn, LockType req_type);
  // Returns the new value of holders after decrementation.
  int decrement_holders();
  void set_next_stage(std::shared_ptr<LockStage> ls);

  // getters
  int get_current_holders();
  std::shared_ptr<LockStage> get_next_request();
  std::unordered_set<Txn*>& get_requesters();
  LockType get_lock_type();
};

#endif // _LOCK_STAGE_H_
