#ifndef _LOCK_STAGE_H_
#define _LOCK_STAGE_H_

#include "lock/lock.h"
#include "txns/txn.h"
#include "utils/debug.h"

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
   {
    ASSERT(!(t == LockType::exclusive && reqers.size() > 1));
   };

  // Returns true if successful and false otherwise.
  bool add_to_stage(std::shared_ptr<Txn> txn, LockType req_type);
  // Returns the new value of holders after decrementation.
  int decrement_holders();
  void set_next_stage(std::shared_ptr<LockStage> ls);

  // getters
  int get_current_holders() const;
  std::shared_ptr<LockStage> get_next_request();
  const std::unordered_set<std::shared_ptr<Txn>>& get_requesters() const;
  LockType get_lock_type() const;

  friend bool operator==(const LockStage& ls1, const LockStage& ls2);
  friend void BatchScheduleInsert();
};

#endif // _LOCK_STAGE_H_
