#include "simulation/txn_map.h"

double TxnMap::get_average_completion_time() {
  double rolling_average = 0.0;
  double current_elt = 0.0;
  for (unsigned int i = 0; i < txns.size(); i++) {
    current_elt = txns[i].start_time + txns[i].exec_duration - txns[i].arrival_time;
    current_elt /= (double) (i + 1);

    rolling_average *= (double) i / (double) (i + 1);
    rolling_average += current_elt;

    assert(rolling_average > 0);
  }

  return rolling_average;
}

void TxnMap::set_start_time(unsigned int txn_id, unsigned int time) {
  assert(id_map.find(txn_id) != id_map.end());

  id_map.find(txn_id)->second->start_time = time;
}

void TxnMap::reset_start_time() {
  for (auto& tw : txns)
   tw.start_time = 0; 
}

std::vector<std::unique_ptr<Txn>> TxnMap::get_txns_between(
    unsigned int lower_arrival_time, unsigned int upper_arrival_time) {
  assert(lower_arrival_time <= upper_arrival_time);
  
  auto lower_it = std::lower_bound(
      txns.begin(),
      txns.end(),
      TxnWrapper(Txn(0), lower_arrival_time, 0),
      TxnWrapperArrivalTimeOrdering);
  auto upper_it = std::upper_bound(
      txns.begin(),
      txns.end(),
      TxnWrapper(Txn(0), upper_arrival_time, 0),
      TxnWrapperArrivalTimeOrdering);

  // make the return vector!
  std::vector<std::unique_ptr<Txn>> result; 
  while (lower_it < upper_it) {
    Txn t = Txn(lower_it->t);
    result.push_back(
      std::make_unique<Txn>(Txn(lower_it->t)));
    std::advance(lower_it, 1);
  }

  return result;
}
