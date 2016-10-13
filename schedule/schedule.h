#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "txns/txn.h"
#include "lock/lock.h"
#include "utils/mutex.h"

#include <condition_variable>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <memory>
#include <mutex>

class Schedule {
private:
  //  The LockStage structure corresponds to a request for a lock
  //  in a given time. See pdf for details.
  //
  //  NOTE:
  //    This is not thread-safe and should be guarded by a lock
  //    before access.
  struct LockStage {
    int holders;
    std::shared_ptr<struct LockStage> next_request;
    std::unordered_set<Txn*> requesters;    
    LockType type; // the type of the lock this corresponds to
    LockStage(): 
      holders(0), 
      next_request(nullptr), 
      requesters({}), 
      type(LockType::shared)
     {}; 

    // returns true if successful and false otherwise
    bool add_to_stage(Txn* txn, LockType req_type);
    // decrements the holders count. Returns the new value of holders.
    int decrement_holders();
    // sets the nextRequest pointer.
    void set_next_stage(std::shared_ptr<struct LockStage> ls);
  };

  struct LockQueue_ {
    std::shared_ptr<struct LockStage> current;
    std::shared_ptr<struct LockStage> newest;
    MutexRW mutex_;

    // thread safe. Returns true if queue was empty at the time of insertion.
    bool insert_into_queue(std::shared_ptr<struct LockStage> stage);
    // returns the set of txns that have all of the required locks after the current
    // stage has been released!
    std::unordered_set<Txn*> finalize_txn_within_current();
    // thread safe. Returns true if queue was empty at the time of insertion.
    bool merge_into_lock_queue(struct LockQueue_& lq);
  };

  struct ReadyTxnQueue {
    std::list<Txn*> ready_txns;
    // we really really want a mutex here because of condition variable usage
    std::mutex mutex_;
    std::condition_variable cv_;

    void add_txn(Txn* t);
    Txn* get_ready_txn();
  };

  // TODO:
  //    This can be allocated with ALL the locks present from the get-go. Profile
  //    and see if that is necessary.
  struct LockQueues {
    struct ReadyTxnQueue ready_txns;
    std::unordered_map<int, std::shared_ptr<struct LockQueue_> > lock_queues;
    // used when new locks are inserted. It's done on the fly!
    MutexRW mutex_;

    // returns the pointer to the lock queue or nullptr if it doesn't exist.
    std::shared_ptr<struct LockQueue_> get_lock_queue(int lck);
    // attempts to insert lockQueue corresponding to lck. 
    //
    // Returns the pointer to the inserted element or the existing elt otherwise.
    std::shared_ptr<struct LockQueue_>  emplace_lock_queue(int lck);
    // return true if lock is inmediately granted and false otherwise.
    bool insert_ex_lock_request(Txn* t, int lck);
    bool insert_sh_lock_request(Txn* t, int lck);

    void finalize_lock_request(int lck);
  };

  struct LockQueues locks_;

  // batch schedules do NOT move items into ready lists and they do NOT
  // notify the transactions of locks that are granted!
  const bool is_batch_schedule;

public:
  Schedule(bool batch_schedule = true): is_batch_schedule(batch_schedule) {};
  // adds txn to the schedule
  void add(Txn *t);
  // finlizes the txn -- means the txn has executed
  void finalize(Txn *t);
  // blocks until a txn is available
  Txn* get_next_txn_to_execute();
  // merge a batch schedule into this schedule.
  void merge_into_schedule(Schedule& s);
};

#endif // _SCHEDULE_H_
