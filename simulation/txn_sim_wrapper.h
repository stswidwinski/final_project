#ifndef _TXN_SIM_WRAPPER_H_
#define _TXN_SIM_WRAPPER_H_

#include "txns/txn.h"

class TxnWrapper {
public:
  TxnWrapper(Txn txn, unsigned int st, unsigned int dur):
    t(txn), start_time(st), exec_duration(dur) {};
  
  Txn t;
  unsigned int start_time;
  unsigned int exec_duration;

  std::unique_ptr<Txn> get_cpy_of_txn();
};

#endif // _TXN_SUM_WRAPPER_H_
