#include "txns/txn.h"
#include "utils/debug.h"

void Txn::add_to_write_set(const int& n) {
  write_set->insert(n);
  increment_locks_needed();
}

void Txn::add_to_write_set(std::unordered_set<int> to_add) {
  for (const auto& elem : to_add) {
    add_to_write_set(elem);
  }
}

void Txn::add_to_read_set(const int& n) {
  read_set->insert(n);  
  increment_locks_needed();
}

void Txn::add_to_read_set(std::unordered_set<int> to_add) {
  for (const auto& elem : to_add) {
    add_to_read_set(elem);
  }
}

std::shared_ptr<std::unordered_set<int>> Txn::get_write_set_handle() const {
  return write_set; 
};

std::shared_ptr<std::unordered_set<int>> Txn::get_read_set_handle() const {
  return read_set;
}

unsigned int Txn::get_id() const {
  return id;
}

bool Txn::operator<(const Txn& txn) const {
  int count = write_set->size() + read_set->size();
  int txn_count = txn.get_write_set_handle()->size() + txn.get_read_set_handle()->size();
  return count < txn_count;
}

bool Txn::lock_granted() {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  locks_granted ++;
  ASSERT(locks_granted <= locks_needed);

  return locks_granted == locks_needed;
}

void Txn::increment_locks_needed() {
  MutexRWGuard write_lock(&mutex_, LockType::exclusive);
  locks_needed ++;

  ASSERT(locks_granted <= locks_needed);
}
