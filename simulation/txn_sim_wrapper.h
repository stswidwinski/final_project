#ifndef _TXN_SIM_WRAPPER_H_
#define _TXN_SIM_WRAPPER_H_

#include "txns/txn.h"

class TxnWrapper {
public:
  TxnWrapper(Txn txn, unsigned int at, unsigned int dur):
    t(txn), arrival_time(at), exec_duration(dur), start_time(0) {};
  
  Txn t;
  unsigned int arrival_time;
  unsigned int exec_duration;
  unsigned int start_time;

  std::unique_ptr<Txn> get_cpy_of_txn();
};

#endif // _TXN_SUM_WRAPPER_H_
