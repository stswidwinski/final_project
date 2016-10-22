#ifndef _ARR_CONTAINER_H_
#define _ARR_CONTAINER_H_

#include "containers/container.cc"
#include "txns/txn.h"

#include <memory>

class ArrayContainer : public Container {
public:
 ArrayContainer() = delete;
 ArrayContainer(const ArrayContainer& ac) = delete;
 ArrayContainer(std::unique_ptr<std::vector<Txn>> txns): 
     Container(std::move(txns)), current_min_index(0), current_barrier_index(0) {
   sort_remaining();
 };

 Txn* get_next_min_elt() override;
 void remove_former_min() override;
 void sort_remaining() override;
 unsigned int get_remaining_count() override;
 virtual ~ArrayContainer() {};

private:
  // general structure of the array:
  //
  //  |--|--|--|--|--|--|--|--|--|--|--|
  //                   ^-- current_min_index
  //       ^----- current_barrier_index
  //  |-----| -- removed elements
  //
  //        |--------| -- inspected, but not removed elements
  //  
  //                 |-----------------| -- not inspected, not removed elements.
  unsigned int current_min_index;
  // we swap elements behind barrier when we stop considering them (when we "remove" them).
  unsigned int current_barrier_index;
};

#endif // _ARR_CONTAINER_H
