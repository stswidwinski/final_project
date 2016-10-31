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

  std::uniform_int_distribution<int> arrival_time_distro;
  std::normal_distribution<double> cont_lock_num_distro;
  std::normal_distribution<double> uncont_lock_num_distro;

  unsigned int lock_time_multiplier;
  unsigned int txn_number;
  unsigned int uncont_lock_space_size;
  unsigned int cont_lock_space_size;
  double write_txns_perc;

  // this is the base chance for putting a txn down given no points.
  double seed_chance;
  // The probability that a point is put in bursty distribution is 
  // seed_chance + 1 / (distance from closest point) * linear_factor;
  unsigned int linear_factor;

  std::vector<int> rand_without_repeats(
    unsigned int how_many,
    unsigned int range_end_with,
    unsigned int range_begin_with = 0);

  std::vector<int> gen_lock_set(
    std::normal_distribution<double>* lock_distro,
    unsigned int lock_space_size,
    unsigned int lock_space_start);

  void check_set_fields();
  TxnWrapper gen_wrapper(unsigned int arrival_time, unsigned int txn_id, bool isWriting);
public:
  TxnGenerator() {
    std::random_device rd;
    rand_gen = std::default_random_engine(rd());
    uniform_probability = std::uniform_real_distribution<double>(0.0, 1.0);
    seed_chance = 0.001;
    linear_factor = 100;
  }
  // the fields set correpsond to those above in order.
  void set_lock_time_mult(unsigned int mult);
  void set_time_period(unsigned int tp);
  void set_txn_num(unsigned int tn);
  // ha - held average, hsd - held standard deviation
  void set_uncont_lock_info(unsigned int ulss, unsigned int ulha, unsigned int ulhsd);
  void set_cont_lock_info(unsigned int clss, unsigned int clha, unsigned int clhsd);
  void set_write_txn_perc(double wtp);
  void set_seed_chance(double seed);

  // All generation functions return a vector sorted on the begin times of txns
  std::vector<TxnWrapper> gen_uniform(); 
  // Expected linear time (linear in the # of txns).
  std::vector<TxnWrapper> gen_bursty();

  friend void RandWithoutRepeatsTest();
};

#endif // _TXN_GENERATOR_H_
