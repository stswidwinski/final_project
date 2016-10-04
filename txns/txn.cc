#include "txns/txn.h"

void Txn::add_to_write_set(int n) {
  write_set->insert(n);
}

void Txn::add_to_write_set(std::unordered_set<int> to_add) {
  for (const auto& elem : to_add)
    write_set->insert(elem);
}

void Txn::add_to_read_set(int n) {
  read_set->insert(n);  
}

void Txn::add_to_read_set(std::unordered_set<int> to_add) {
  for (const auto& elem : to_add)
    read_set->insert(elem);
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


