#ifndef _TXN_GENERATOR_H_
#define _TXN_GENERATOR_H_

#include "txns/txn.h"
#include "simulation/txn_sim_wrapper.h"

#include <memory>
#include <random>
#include <cassert>
#include <vector>

class TxnGenerator {
private:
  std::default_random_engine rand_gen;
  std::uniform_real_distribution<double> uniform_probability;
  unsigned int lock_time_multiplier;
  unsigned int time_period;
  unsigned int txn_number;
  unsigned int uncont_lock_space_size;
  unsigned int uncont_locks_held_avg;
  unsigned int uncont_locks_held_std_dev;
  unsigned int cont_lock_space_size;
  unsigned int cont_locks_held_avg;
  unsigned int cont_locks_held_std_dev;
  double write_txns_perc;

  std::vector<int> rand_without_repeats(
    unsigned int how_many,
    unsigned int range_end_with,
    unsigned int range_begin_with = 0);

  std::vector<int> gen_lock_set(
    std::normal_distribution<double>* lock_distro,
    unsigned int lock_space_size,
    unsigned int lock_space_start);

public:
  TxnGenerator() {
    std::random_device rd;
    rand_gen = std::default_random_engine(rd());
    uniform_probability = std::uniform_real_distribution<double>(0.0, 1.0);
  }
  // the fields set correpsond to those above in order.
  void set_lock_time_mult(unsigned int mult);
  void set_time_period(unsigned int tp);
  void set_txn_num(unsigned int tn);
  void set_uncont_lock_info(unsigned int ulss, unsigned int slha, unsigned int ulhsd);
  void set_cont_lock_info(unsigned int clss, unsigned int clha, unsigned int clhsd);
  void set_write_txn_perc(double wtp);

  // All generation functions return a vector sorted on the begin times of txns
  std::vector<TxnWrapper> gen_uniform(); 

  friend void RandWithoutRepeatsTest();
};

#endif // _TXN_GENERATOR_H_
