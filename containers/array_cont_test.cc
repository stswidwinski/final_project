#include "containers/array_cont.h"
#include "txns/txn.h"
#include "utils/testing.h"

#include <iostream>
#include <memory>
#include <vector>

TEST(ArrayContainerTest) {
  // prepare the vector and container
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  for (int i = 0; i < 100; i++) {
    Txn t;
    for (int j = 0; j < i; j++) {
      t.add_to_write_set(j);
    }
    v->push_back(t);
  }

  ArrayContainer c(std::move(v));
  
  // get consequent minima and check that the sorting is correct. 
  // remove every other element.
  for (unsigned int i = 0; i < 100; i++) {
    EXPECT_EQ(
      i,
      c.get_next_min_elt()->get_write_set_handle()->size());

    if ( (i % 2) == 0) {
      c.remove_former_min();
    }
  }

  // the end has been reached, must be 0x0.
  EXPECT_EQ(0x0, c.get_next_min_elt());
  
  c.sort_remaining();
  // half of the elements should have been "removed"
  for (unsigned int i = 1; i < 100; i += 2 ) {
    EXPECT_EQ(
        i,
        c.get_next_min_elt()->get_write_set_handle()->size());
    
    c.remove_former_min();   
  }

  // the end should be reached. 0x0.
  EXPECT_EQ(0x0, c.get_next_min_elt());
 
  // sorting empty list still works and we still get 0x0.
  c.sort_remaining();
  EXPECT_EQ(0x0, c.get_next_min_elt());
  END;
}

int main(int argc, char** argv) {
  ArrayContainerTest();
}
