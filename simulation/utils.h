#include "simulation/txn_sim_wrapper.h"
#include "utils/debug.h"

#include <vector>
#include <iostream>
#include <fstream>

void write_txn_load(std::string file_name, std::vector<TxnWrapper> txn_load) {
  ofstream dump_file;
  try {
    dump_file.open(file_name, ios::out | ios::trunc);
  } catch (const ofstream::failure& e) {
    DEBUG_CERR(std::cerr << e.what() << ". ERR CODE:" << e.code() << std::endl;);
  }

  dump_file << "start_time" << "," << "duration\n";
  for (const TxnWrapper& tw : txn_load) {
    dump_file << tw.start_time << "," << tw.exec_duration << "\n";
  }

  dump_file.close();
}
