#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "txns/txn.h"

#include <memory>
#include <vector>

class Container {
protected:
  Container() = delete;
  Container(const Container& c) = delete;
  Container(std::unique_ptr<std::vector<std::unique_ptr<Txn>>> txns): 
    txn_arr(std::move(txns)) 
  {};

public:
  // get an observing pointer to the currently minimum element.
  // Returns nullptr if no elements left.
  virtual Txn* peak_curr_min_elt() = 0;
  // get an owning pointer to the currently minimum element. The element
  // is removed from the container. Obviously, we also move on to the next
  // element. 
  //
  // Returns nullptr if no elements left.
  virtual std::unique_ptr<Txn> take_curr_min_elt() = 0;
  // advance to the next minimum element. 
  virtual void advance_to_next_min() = 0;
  // sort the elements still within the container.
  virtual void sort_remaining() = 0;
  // get the number of elements still within the container
  virtual unsigned int get_remaining_count() = 0;

  std::unique_ptr<std::vector<std::unique_ptr<Txn>>> txn_arr;
  
  virtual ~Container(){};
};

#endif // _CONTAINER_H_
