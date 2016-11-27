#include "simulation/schedule_snooper.h"
#include "utils/debug.h"

#include <fstream>
#include <set>
#include <memory>

void ScheduleSnooper::update_snooper(BatchSchedule *bs) {
  // we would like to remember the first lock stage for every lock. We 
  // only add when we don't have something that a batch contains.
  for (auto& elt : bs->lock_table.lock_table) {
    auto it = lock_stages.emplace(
        elt.first, 
        std::vector<std::shared_ptr<LockStage>>{elt.second->current});

    //  If we already have an element under this "address", but its pointer 
    //  to the next elt is nullptr, it means that the current txn has finished
    //  before the next came in. Hence, there should be no dependencies between
    //  them. That said, we should have both of the elements present in the
    //  final graph.
    if (it.second == false &&
        it.first->second.back()->get_next_request() == nullptr) {
      it.first->second.push_back(elt.second->current);
    }
  }
}

void ScheduleSnooper::print_dependencies(std::string dump_path, std::string model_name) {
  std::unordered_map<txn_id, std::shared_ptr<Txn>> txns;
  std::unordered_map<txn_id, std::set<txn_id>> dependencies;
  std::shared_ptr<LockStage> current;
  std::shared_ptr<LockStage> next;
  static std::unordered_map<std::string, int> counters;
  
  for (auto elt : lock_stages) {
    for (auto current : elt.second) {
      while (current != nullptr) {
        next = current->get_next_request();
        for (auto txn_ptr : current->get_requesters()) {
          txns.insert({txn_ptr->get_id(), txn_ptr});
          auto it = dependencies.emplace(txn_ptr->get_id(), std::set<txn_id>());
          if (next != nullptr) {
            ASSERT(
                !(next->get_lock_type() == LockType::exclusive && 
                  next->get_requesters().size() > 1));
            for (auto dep_pt : next->get_requesters()) {
              it.first->second.insert(dep_pt->get_id());
            }
          }
        }

        current = next;
      }
    }
  }

  // dependencies should now be generated.
  auto it = counters.emplace(model_name, 0);
  std::string path = dump_path + '/' + "dep_graph_" + model_name + std::to_string(it.first->second ++); 
  std::ofstream dump_file;
  try {
    dump_file.open(path, std::ios::out | std::ios::trunc);
  } catch (const std::ofstream::failure& e) {
    DEBUG_CERR(std::cerr << e.what()  << ". ERR CODE: " << e.code() << std::endl;);
  }

  // preamble
  dump_file << "blockdiag {\n";
  dump_file << "class excl [color=red];\nclass shar [color=green];\n";
  for (auto& elt : dependencies) {
    // set the node to the appropriate style
    dump_file << std::to_string(elt.first) << " ";
    if (txns.find(elt.first)->second->get_write_set_handle()->size() == 0) {
      dump_file << "[class = \"shar\"]\n";
    } else {
      dump_file << "[class = \"excl\"]\n";
    }
    dump_file << std::to_string(elt.first);
    bool first = true;
    for (auto id : elt.second) {
      if (first) {
        dump_file << " -> " << std::to_string(id);
        first = false;
        continue;
      }
      dump_file << ", " << std::to_string(id);
    }
    dump_file << ";\n";
  }
  dump_file << "\n}";
  dump_file.close();
}
