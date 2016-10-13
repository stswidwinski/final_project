#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <iostream>

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
#define ASSERT(X)                                  \
  do { (void)(sizeof(X)); } while (0)              
#endif // DEBUG

#endif // _DEBUG_H_
