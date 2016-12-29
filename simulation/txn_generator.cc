#include "simulation/txn_generator.h"

#include <limits>

// both ranges are inclusive
std::vector<int> TxnGenerator::rand_without_repeats(
    unsigned int how_many,
    unsigned int range_length,
    unsigned int range_begin_with) {
  std::vector<int> pos;
  assert(how_many > 0);
  assert(range_length >= 0);
  assert(range_length >= how_many);

  for (unsigned int i = 0; i < range_length; i++) {
    pos.push_back((int) (i + range_begin_with));
  }

  std::vector<int> res;
  unsigned int tmp_rand;
  for (unsigned  int i = 0; i < how_many; i++) {
    tmp_rand = (std::uniform_int_distribution<int> (0, range_length - 1 - i))(rand_gen);
    res.push_back(pos[tmp_rand]);
    // swap the element to the back
    pos[tmp_rand] = pos[range_length - 1 - i];
  }

  return res;
}

std::vector<int> TxnGenerator::gen_lock_set(
    std::normal_distribution<double>* lock_distro,
    unsigned int lock_space_size,
    unsigned int lock_space_start) {
  if (lock_distro->mean() == 0)
    return std::vector<int>{};

  double lock_number;
  // make sure that we aren't unlucky and the number is actually positive
  while((lock_number = (*lock_distro)(rand_gen)) < 1) {};

  return rand_without_repeats(
      (unsigned int) lock_number,
      lock_space_size,
      lock_space_start);
}

void TxnGenerator::set_lock_time_mult(unsigned int mult) {
  lock_time_multiplier = mult;
}

void TxnGenerator::set_time_period(unsigned int tp) {
  arrival_time_distro = std::uniform_int_distribution<int>(0, tp - 1);
}

void TxnGenerator::set_txn_num(unsigned int tn) {
  txn_number = tn;
}

void TxnGenerator::set_uncont_lock_info(
    unsigned int ulss, unsigned int ulha, unsigned int ulhsd) {
  uncont_lock_space_size = ulss;
  uncont_lock_num_distro = std::normal_distribution<double>(ulha, ulhsd);
}

void TxnGenerator::set_cont_lock_info(
    unsigned int clss, unsigned int clha, unsigned int clhsd) {
  cont_lock_space_size = clss;
  cont_lock_num_distro = std::normal_distribution<double>(clha, clhsd);
}

void TxnGenerator::set_write_txn_perc(double wtp) {
  write_txns_perc = wtp;
}

void TxnGenerator::set_seed_chance(double seed) {
  seed_chance = seed;
}

void TxnGenerator::set_linear_factor(unsigned int lf) {
  linear_factor = lf;
}

void TxnGenerator::set_params(TxnGeneratorParams params) {
  set_lock_time_mult(params.lock_time_multiplier);
  set_txn_num(params.txn_number);
  set_time_period(params.time_period);
  set_uncont_lock_info(
      params.uncont_lock.lock_space,
      params.uncont_lock.avg_locks_held,
      params.uncont_lock.std_dist_of_locks_held);
  set_cont_lock_info(
      params.cont_lock.lock_space,
      params.cont_lock.avg_locks_held,
      params.cont_lock.std_dist_of_locks_held);
  set_write_txn_perc(params.write_txns_perc);
  set_seed_chance(params.seed_chance);
  set_linear_factor(params.linear_factor);
}

void TxnGenerator::check_set_fields() {
  // bare minimum checks for initialization of parameters.
  assert(write_txns_perc <= 1.0 && write_txns_perc >= 0.0);
  assert(lock_time_multiplier != 0);
  assert(
      std::numeric_limits<int>::max() != arrival_time_distro.b());
  assert(0 == arrival_time_distro.a());
  assert(txn_number != 0);
}

TxnWrapper TxnGenerator::gen_wrapper(
    unsigned int arrival_time,
    unsigned int txn_id,
    bool isWritting) {
  // contended locks are numbered 0 through contended_lock_space_size - 1
  auto cont_locks = gen_lock_set(
      &cont_lock_num_distro, 
      cont_lock_space_size,
      0);
  // contended_lock_space_size + uncontended_lock_space - 1;
  // uncontended locks are numbered contended_lock_space_size through 
  auto uncont_locks = gen_lock_set(
      &uncont_lock_num_distro,
      uncont_lock_space_size,
      cont_lock_space_size);

  // create the actual object to return
  TxnWrapper wrapper = TxnWrapper(
      Txn(txn_id),
      arrival_time,
      0);

  // add to the correct set
  void (Txn::*add_to_set)(const int&) = isWritting ?
    (void (Txn::*)(const int&)) &Txn::add_to_write_set : 
    (void (Txn::*)(const int&)) &Txn::add_to_read_set;
  for (auto& lock : cont_locks)
    (wrapper.t.*add_to_set)(lock);
  for (auto& lock : uncont_locks)
    (wrapper.t.*add_to_set)(lock);
  
  wrapper.exec_duration = 
    (cont_locks.size() + uncont_locks.size()) * lock_time_multiplier;

  return wrapper;
}

std::vector<TxnWrapper> TxnGenerator::gen_uniform() {
  check_set_fields();
  std::vector<TxnWrapper> result; 

  // First write txns and then read txns
  for (unsigned int i = 0; i < (unsigned int) (write_txns_perc * txn_number); i++) {
    result.push_back(gen_wrapper(arrival_time_distro(rand_gen), i, true));
  }

  unsigned int lacking_txns = txn_number - result.size();
  for (unsigned int i = 0; i < lacking_txns; i++) {
    result.push_back(gen_wrapper(arrival_time_distro(rand_gen), result.size(), false));
  }

  return result;
};

std::vector<TxnWrapper> TxnGenerator::gen_bursty() {
  check_set_fields();
  auto txn_wrapper_ordering = [](const TxnWrapper& t1, const TxnWrapper& t2) {
    return (t1.arrival_time < t2.arrival_time);
  };
  std::multiset<TxnWrapper, decltype(txn_wrapper_ordering)> result_set(txn_wrapper_ordering); 
  // find a txn within result that is closest by arrival time. We use that for the
  // probability generation.
  auto find_closest_txn_dist = [&result_set](unsigned int arrival_time){
    std::multiset<TxnWrapper>::iterator close_up = 
      result_set.lower_bound(TxnWrapper(Txn(1), arrival_time, 1));
    
    if (close_up == result_set.end()) {
      return std::numeric_limits<unsigned int>::max();
    } else if (close_up == result_set.begin()) {
      // one-sided!
      return (unsigned int) std::abs(close_up->arrival_time - arrival_time);
    } else {
      // two-sided!
      std::multiset<TxnWrapper>::iterator close_low = close_up;
      close_low --;

      unsigned int dist1 = std::abs(close_up->arrival_time - arrival_time);
      unsigned int dist2 = std::abs(arrival_time - close_low->arrival_time);

      return (dist1 >= dist2) ? dist2 : dist1;
    }
  };
  // for bursty work-loads we must skew the distribution to make it clustered.
  auto get_arrival_time = [this, &find_closest_txn_dist](){
    unsigned int arrival_time = 0;
   // for (unsigned int fails = 0; fails < max_failed_attempt; fails++) {
   while(true) {  
      arrival_time = arrival_time_distro(rand_gen);
      double probability = seed_chance + double(linear_factor) / find_closest_txn_dist(arrival_time);
      if (uniform_probability(rand_gen) < probability) {
        return arrival_time;
      }
    }
  };

  // As in uniform, first write then read
  for (unsigned int i = 0; i < (unsigned int) (write_txns_perc * txn_number); i++) {
    // for bursty work loads we must skew the distribution!
    result_set.insert(gen_wrapper(get_arrival_time(), i, true));
  }

  unsigned int lacking_txns = txn_number - result_set.size();
  for (unsigned int i = 0; i < lacking_txns; i++) {
    result_set.insert(gen_wrapper(get_arrival_time(), result_set.size(), false));
  }

  // convert the result to an actual vector...
  std::vector<TxnWrapper> result;
  for (auto& tw : result_set) {
    result.push_back(std::move(tw));
  }

  return result;
}
