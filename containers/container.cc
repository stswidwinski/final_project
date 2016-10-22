#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "txns/txn.h"
#include <vector>

class Container {
protected:
  Container() = delete;
  Container(const Container& c) = delete;
  Container(std::unique_ptr<std::vector<Txn>> txns): txn_arr(std::move(txns)) {};

public:
  // get the minimal txn as dictated by >= of Txn.
  // Returns nullptr if no elements left.
  virtual Txn* get_next_min_elt() = 0;
  // remove the former minimum from the container. Does not guarantee 
  // that the memory will be freed.
  virtual void remove_former_min() = 0;
  // sort the elements still within the container.
  virtual void sort_remaining() = 0;
  // get the number of elements still within the container
  virtual unsigned int get_remaining_count() = 0;

  std::unique_ptr<std::vector<Txn>> txn_arr;
  
  virtual ~Container(){};
};

#endif // _CONTAINER_H_
