#include "simulation/txn_generator.h"

// both ranges are inclusive
std::vector<int> TxnGenerator::rand_without_repeats(
    unsigned int how_many,
    unsigned int range_length,
    unsigned int range_begin_with) {
  std::vector<int> pos;
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

  double lock_number;
  // make sure that we aren't unlucky and the number is actually positive
  while((lock_number = (*lock_distro)(rand_gen)) < 0) {};

  return rand_without_repeats(
      (unsigned int) lock_number,
      lock_space_size,
      lock_space_start);
}

void TxnGenerator::set_lock_time_mult(unsigned int mult) {
  lock_time_multiplier = mult;
}

void TxnGenerator::set_time_period(unsigned int tp) {
  time_period = tp;
}

void TxnGenerator::set_txn_num(unsigned int tn) {
  txn_number = tn;
}

void TxnGenerator::set_uncont_lock_info(
    unsigned int ulss, unsigned int slha, unsigned int ulhsd) {
  uncont_lock_space_size = ulss;
  uncont_locks_held_avg = slha;
  uncont_locks_held_std_dev = ulhsd;
}

void TxnGenerator::set_cont_lock_info(
    unsigned int clss, unsigned int clha, unsigned int clhsd) {
  cont_lock_space_size = clss;
  cont_locks_held_avg = clha;
  cont_locks_held_std_dev = clhsd;
}

void TxnGenerator::set_write_txn_perc(double wtp) {
  write_txns_perc = wtp;
}

std::vector<TxnWrapper> TxnGenerator::gen_uniform() {
  // bare minimum checks for initialization of parameters.
  assert(write_txns_perc <= 1.0 && write_txns_perc >= 0.0);
  assert(lock_time_multiplier != 0);
  assert(time_period != 0);
  assert(txn_number != 0);
  // first, generate the transactions by type by the beginning time. Add:
  //    # of locks
  //    length of txn based on # of locks.
  std::uniform_int_distribution<int> start_time_distro(0, time_period - 1);
  std::normal_distribution<double> cont_lock_num_distro(
      cont_locks_held_avg,
      cont_locks_held_std_dev);
  std::normal_distribution<double> uncont_lock_num_distro(
      uncont_locks_held_avg,
      uncont_locks_held_std_dev);
  std::vector<TxnWrapper> result; 

  auto gen_wrapper = [
      this, 
      &start_time_distro, 
      &cont_lock_num_distro,
      &uncont_lock_num_distro]
        (bool isWriting, unsigned int txn_id) {
    unsigned int start_time = start_time_distro(rand_gen);
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
        start_time,
        0);

    // add to the correct set
    void (Txn::*add_to_set)(const int&) = isWriting ? 
      (void (Txn::*)(const int&)) &Txn::add_to_write_set : 
      (void (Txn::*)(const int&)) &Txn::add_to_read_set;
    for (auto& lock : cont_locks)
      (wrapper.t.*add_to_set)(lock);
    for (auto& lock : uncont_locks)
      (wrapper.t.*add_to_set)(lock);
    
    wrapper.exec_duration = 
      (cont_locks.size() + uncont_locks.size()) * lock_time_multiplier;

    return wrapper;
  };

  // after a lengthy setup, generate the transactions!
  // First write txns and then read txns
  for (unsigned int i = 0; i < (unsigned int) (write_txns_perc * txn_number); i++) {
    result.push_back(gen_wrapper(true, i));
  }

  unsigned int lacking_txns = txn_number - result.size();
  unsigned int current_txn_num;
  for (unsigned int i = 0; i < lacking_txns; i++) {
    current_txn_num = result.size();
    result.push_back(gen_wrapper(false, current_txn_num));
  }

  return result;
};

