#include "containers/array_cont.h"
#include "txns/txn.h"
#include "utils/testing.h"

#include <iostream>
#include <memory>
#include <vector>

TEST(ArrayContainerTest) {
  // prepare the vector and container
  std::unique_ptr<std::vector<std::unique_ptr<Txn>>> v = 
    std::make_unique<std::vector<std::unique_ptr<Txn>>>();
  for (int i = 0; i < 100; i++) {
    std::unique_ptr<Txn> t = std::make_unique<Txn>(i);
    for (int j = 0; j < i; j++) {
      t->add_to_write_set(j);
    }
    v->push_back(std::move(t));
  }

  ArrayContainer c(std::move(v));
  
  // get consequent minima and check that the sorting is correct. 
  // remove every other element.
  for (unsigned int i = 0; i < 100; i++) {
    auto tmp = c.peak_curr_min_elt();
    EXPECT_EQ(i, tmp->get_write_set_handle()->size());
    EXPECT_EQ(i, tmp->get_id());

    if ( (i % 2) == 0) {
      EXPECT_EQ(
          100 - i/2,
          c.get_remaining_count());
      auto min = c.take_curr_min_elt();
      EXPECT_EQ(min->get_id(), i);
      EXPECT_EQ(
          100 - i/2 - 1,
          c.get_remaining_count());
    } else {
      c.advance_to_next_min();
    }
  }

  // the end has been reached, must be 0x0.
  EXPECT_EQ(0x0, c.peak_curr_min_elt());
  
  c.sort_remaining();
  // half of the elements should have been "removed"
  for (unsigned int i = 1; i < 100; i += 2 ) {
    auto tmp = c.peak_curr_min_elt();
    EXPECT_EQ(i, tmp->get_write_set_handle()->size());
    EXPECT_EQ(i, tmp->get_id());

    auto min = c.take_curr_min_elt();
    EXPECT_EQ(i, min->get_id());
  }

  // the end should be reached. 0x0.
  EXPECT_EQ(0x0, c.peak_curr_min_elt());
 
  // sorting empty list still works and we still get 0x0.
  c.sort_remaining();
  EXPECT_EQ(0x0, c.peak_curr_min_elt());
  END;
}

TEST(GetTxnByIdTest) {
   // prepare the vector and container
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  for (int i = 0; i < 100; i++) {
    Txn t(i);
    for (int j = 0; j < i; j++) {
      t.add_to_write_set(j);
    }
    v->push_back(t);
  }

  ArrayContainer c(std::move(v));
  for (int i = 99; i >= 0; i--) {
    EXPECT_EQ(
      (unsigned int) i,
      c.get_txn_by_id(i)->get_id());
  }

  EXPECT_TRUE(nullptr == c.get_txn_by_id(100));
  END;
}

int main(int argc, char** argv) {
  ArrayContainerTest();
}
