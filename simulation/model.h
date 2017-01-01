#ifndef _SIM_MODEL_H_
#define _SIM_MODEL_H_

#include <set>
#include <string>

// the following are the legal models
enum class Model {
  sequential,
  real_time,
  batched,
  model_types_count
};

// get models from user input
std::set<Model> get_models(int argc, char** argv);
std::string model_to_string(Model model);
#endif // _SIM_MODEL_H_
