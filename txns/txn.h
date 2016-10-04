#ifndef _TXN_H_
#define _TXN_H_

#include <unordered_set>
#include <memory>

class Txn {
public:
  Txn(unsigned int i) {
    write_set = std::make_shared<std::unordered_set<int>>();
    read_set = std::make_shared<std::unordered_set<int>>();
    id = i;
  }

  void add_to_write_set(int n);
  void add_to_read_set(int n);
  void add_to_write_set(std::unordered_set<int> to_add);
  void add_to_read_set(std::unordered_set<int> to_add);

  std::shared_ptr<std::unordered_set<int>> get_write_set_handle() const;
  std::shared_ptr<std::unordered_set<int>> get_read_set_handle() const;

  unsigned int get_id() const;
  // TODO:
  //  Consider different ways to compare. The size of combined read and write 
  //  sets, the size of write set only etc.
  bool operator<(const Txn& txn) const;

private:
  std::shared_ptr<std::unordered_set<int>> write_set;
  std::shared_ptr<std::unordered_set<int>> read_set;

  unsigned int id;
};

#endif // _TXN_H_
