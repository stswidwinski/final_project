#include "simulation/txn_map.h"
#include "utils/debug.h"

#include <math.h>

double TxnMap::get_average_completion_time() {
  double rolling_average = 0.0;
  double current_elt = 0.0;
  for (unsigned int i = 0; i < txns.size(); i++) {
    current_elt = txns[i].start_time + txns[i].exec_duration - txns[i].arrival_time;
    current_elt /= (double) (i + 1);

    rolling_average *= (double) i / (double) (i + 1);
    rolling_average += current_elt;

    ASSERT(rolling_average >= 0);
  }

  return rolling_average;
}

double TxnMap::get_std_deviation_completion_time() {
  double average = get_average_completion_time();
  double std_avg;

  double size = txns.size();
  // first, get the sum
  for (unsigned int i = 0; i < size; i++) {
    auto cmp_time = txns[i].start_time + txns[i].exec_duration - txns[i].arrival_time;
    std_avg += pow(cmp_time - average, 2) / size;
  }

  return sqrt(std_avg);
}

void TxnMap::set_start_time(unsigned int txn_id, unsigned int time) {
  assert(id_map.find(txn_id) != id_map.end());

  id_map.find(txn_id)->second->start_time = time;
}

void TxnMap::reset_start_time() {
  for (auto& tw : txns)
   tw.start_time = 0; 
}

std::vector<std::unique_ptr<Txn>> TxnMap::get_cpy_of_txns_between(
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

unsigned int TxnMap::get_completion_time(unsigned int txn_id) {
  auto elt = id_map.find(txn_id);
  assert(elt != id_map.end());

 return elt->second->start_time + elt->second->exec_duration; 
}
