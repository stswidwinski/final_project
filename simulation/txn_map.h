#ifndef _TXN_MAP_H_
#define _TXN_MAP_H_

#include "simulation/txn_sim_wrapper.h"
#include "simulation/txn_generator.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>
#include <unordered_map>

class TxnMap {
private:
  std::vector<TxnWrapper> txns;
  std::unordered_map<unsigned int, TxnWrapper*> id_map;

  // inline comparison function.
  static bool TxnWrapperArrivalTimeOrdering(const TxnWrapper& t1, const TxnWrapper& t2) {
    return t1.arrival_time < t2.arrival_time;
  }
public:
  TxnMap(std::vector<TxnWrapper> t) {
    // just to make sure we sort the vector by arrival times!
    std::sort(t.begin(), t.end(), TxnWrapperArrivalTimeOrdering);
    txns = t;

    for (auto& tw : txns) {
      assert(id_map.find(tw.t.get_id()) == id_map.end());
      id_map.insert(std::pair<unsigned int, TxnWrapper*>(tw.t.get_id(), &tw));
    }
  };


  double get_average_completion_time();
  double get_std_deviation_completion_time();
  // lower_arrival_time and upper_arrival_time are inclusive.
  std::vector<std::unique_ptr<Txn>> get_cpy_of_txns_between(
      unsigned int lower_arrival_time, unsigned int upper_arrival_time);
  unsigned int get_completion_time(unsigned int txn_id);
  
  void set_start_time(unsigned int txn_id, unsigned int time);
  void reset_start_time();

  friend void write_txn_load(std::string, std::string, TxnGeneratorParams, TxnMap);
};

#endif // _TXN_MAP_H_
