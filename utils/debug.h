#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <iostream>
#include <stdio.h>

#ifdef DEBUG
#define ASSERT(X)                                  \
  do {                                             \
    if (!(X)) {                                    \
      std::cout << "Assertion " << #X              \
                << " failed at " << __FILE__       \
                << " line " << __LINE__ << "\n";   \
    }                                              \
  } while (0)                                      


#else
#define ASSERT(X)                                  
#endif // DEBUG

#define DEBUG_CERR(X)                              \
  do {                                             \
    std::cerr << __FILE__ << " " << __LINE__ <<    \
      ": ";                                        \
    X                                              \
  } while (0)

#endif // _DEBUG_H_
