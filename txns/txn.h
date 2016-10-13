#ifndef _TXN_H_
#define _TXN_H_

#include "lock/lock.h"
#include "utils/mutex.h"

#include <memory>
#include <set>
#include <unordered_set>

class Txn {
public:
  Txn(unsigned int i): locks_granted(0), locks_needed(0) {
    write_set = std::make_shared<std::set<int>>();
    read_set = std::make_shared<std::set<int>>();
    id = i;
  }

  void add_to_write_set(const int& n);
  void add_to_read_set(const int& n);
  void add_to_write_set(std::unordered_set<int> to_add);
  void add_to_read_set(std::unordered_set<int> to_add);

  std::shared_ptr<std::set<int>> get_write_set_handle() const;
  std::shared_ptr<std::set<int>> get_read_set_handle() const;

  unsigned int get_id() const;
  // TODO:
  //  Consider different ways to compare. The size of combined read and write 
  //  sets, the size of write set only etc.
  bool operator<(const Txn& txn) const;

  // returns true if all locks have been granted and false otherwise.
  bool lock_granted();

private:
  std::shared_ptr<std::set<int>> write_set;
  std::shared_ptr<std::set<int>> read_set;

  unsigned int id;

  // mutex for these two variables.
  MutexRW mutex_;
  unsigned int locks_granted;
  unsigned int locks_needed;

  void increment_locks_needed();
};

#endif // _TXN_H_
