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
  // reset the container to the state at which it was created.
  // Performance is better than that of creating a new object.
  virtual void sort_remaining() = 0;

  std::unique_ptr<std::vector<Txn>> txn_arr;
  
  virtual ~Container(){};
};

#endif // _CONTAINER_H_
