#include <iostream>
#include "containers/array_cont.h"
#include <vector>
#include "txns/txn.h"
#include <memory>

//TODO: 
//  Convert to google unit test-like structure

int main() {
  std::unique_ptr<std::vector<Txn>> v = std::make_unique<std::vector<Txn>>();
  for (int i = 0; i < 100; i++) {
    Txn t;
    for (int j = 0; j < i; j++) {
      t.add_to_write_set(j);
    }
    v->push_back(t);
  }

  ArrayContainer c(std::move(v));
  
  unsigned int res;
  for (unsigned int i = 0; i < 100; i++) {
    res = c.get_next_min_elt()->get_write_set_handle()->size();
     if(res != i) {
      std::cout << "Test failed at first loop.\n";
      std::cout << "expected:" << i << " got " << res << std::endl;
      return 1;
    }

    if ( (i % 2) == 0) {
      c.remove_former_min();
    }
  }

  // now, we should get nullptr!
  if (c.get_next_min_elt() != nullptr) {
    std::cout << "Test failed.\n";
    return 1;
  }
  
  c.sort_remaining();
  // half of the elements should have been "removed"
  for (unsigned int i = 1; i < 100; i += 2 ) {
    if (c.get_next_min_elt()->get_write_set_handle()->size() != i) {
      std::cout << "Test failed.\n";
      return 1;
    }
    
    c.remove_former_min();   
  }

  // now, we should get nullptr!
  if (c.get_next_min_elt() != nullptr) {
    std::cout << "Test failed.\n";
    return 1;
  }
    
  c.sort_remaining();
  // still nullptr!
  if (c.get_next_min_elt() != nullptr) {
    std::cout << "Test failed.\n";
    return 1;
  }

  std::cout << "Test passed\n";
  return 0;
}
