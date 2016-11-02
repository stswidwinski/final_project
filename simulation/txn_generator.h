#ifndef _TXN_GENERATOR_H_
#define _TXN_GENERATOR_H_

#include "txns/txn.h"
#include "simulation/txn_sim_wrapper.h"

#include <chrono>
#include <memory>
#include <random>
#include <cassert>
#include <vector>

/*
 *  Class used to generate transactions for simulations.
 *
 *  The following assumptions hold, independent of the type of the load
 *  we are considering (bursty of uniform):
 *    1) The number of locks held by every transaction is described by
 *      a normal distribution with average and standard deviation set
 *      by the user.
 *
 *      This is true for both the contested and uncontested sets of locks.
 *      Contested and uncontested sets are here to simulate lock contention,
 *      but they may be used in whatever way.
 *      
 *    2) The duration of a transaction execution is described as
 *      
 *      (total # of held locks) * linear_factor 
 *      
 *      Where the linear factor is a parameter set by the user.
 *
 *  GENERATION OF UNIFORM WORKLOAD
 *
 *  The uniform load generation is as one would expect: use a uniform integral distribution
 *  from 0 to time_period - 1 and randomize txn_number arrival times. For each arrival 
 *  time, randomize the number of locks held and the locks uniformly at random from the 
 *  corresponding space of locks (contested/uncontested). The assumption is that the 
 *  contested locks come before uncontested locks. Finally, the execution length is 
 *  linearly dependent, as mentioned before, on the # of locks held (total). 
 *
 *  GENERATION OF BURSTY WORKLOAD
 *
 *  Bursty workload is not generated according to any mathematically well-defined
 *  distribution. The algorithm goes as follows:
 *
 *  For each transaction to be created:
 *      1) Uniformly at random obtain an arrival time t.
 *      2) Find a transaction already added to the workload whose closest to t (arrival-time 
 *          wise)
 *      3) Let P = seed_chance + linear_factor / t. Note that P is usually within the range 
 *          [0, 1] and we don't really care about larger values, because of step 4.
 *      4) Let p = uniform_random_real(0, 1)
 *      5) If p < P, we proceed to create a transaction at this arrival time. Otherwise,
 *          repeat steps 1 through 4 until this holds. 
 *
 *  This simple expected-polynomial time randomized algorithm will create schedules that, 
 *  with high probability, create "bursty" transaction patterns. The formal proof of this 
 *  will not be shown, but visual inspection of time histograms confirms this.
 *
 */

struct LockDistrInfo {
  unsigned int lock_space;
  unsigned int avg_locks_held;
  unsigned int std_dist_of_locks_held;
};

struct TxnGeneratorParams {
  unsigned int lock_time_multiplier;
  unsigned int txn_number;
  unsigned int time_period;
  LockDistrInfo cont_lock;
  LockDistrInfo uncont_lock;
  double write_txns_perc;
  double seed_chance;
  unsigned int linear_factor;
};

class TxnGenerator {
private:
  std::random_device rand_gen;
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
    uniform_probability = std::uniform_real_distribution<double>(0.0, 1.0);
    seed_chance = 0.001;
    linear_factor = 100;
  }
  
  TxnGenerator(TxnGeneratorParams params) {
    TxnGenerator();
    set_params(params);
  };

  // the fields set correpsond to those above in order.
  void set_lock_time_mult(unsigned int mult);
  void set_time_period(unsigned int tp);
  void set_txn_num(unsigned int tn);
  // ha - held average, hsd - held standard deviation
  void set_uncont_lock_info(unsigned int ulss, unsigned int ulha, unsigned int ulhsd);
  void set_cont_lock_info(unsigned int clss, unsigned int clha, unsigned int clhsd);
  void set_write_txn_perc(double wtp);
  void set_seed_chance(double seed);
  void set_linear_factor(unsigned int lf);
  // umbrella for everything
  void set_params(TxnGeneratorParams params);

  // All generation functions return a vector sorted on the begin times of txns
  std::vector<TxnWrapper> gen_uniform(); 
  // Expected linear time (linear in the # of txns).
  std::vector<TxnWrapper> gen_bursty();

  friend void RandWithoutRepeatsTest();
};

#endif // _TXN_GENERATOR_H_
