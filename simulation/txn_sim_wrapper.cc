#include "simulation/txn_sim_wrapper.h"

std::unique_ptr<Txn> TxnWrapper::get_cpy_of_txn() {
  return std::make_unique<Txn>(new Txn(t));
}
