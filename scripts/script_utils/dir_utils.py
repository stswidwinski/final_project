import os
import script_utils.print as p

def get_top_proj_level_dir():
    path = os.path.dirname(os.path.realpath(__file__))
    return path[: path.rfind("scheduling") + len("scheduling")]

def get_raw_data_dir():
    top_level = get_top_proj_level_dir()
    return os.path.join(top_level, "data/raw_data", "")

def get_proc_data_dir():
    top_level = get_top_proj_level_dir()
    return os.path.join(top_level, "data/proc_data", "")

def create_dir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)
        return
   
    os.makedirs(directory)
    p.put("[ FAIL ]\n", "red")
    raise NameError('Directory ' + directory + 'already exists!')

## The general structure of the files for an experiment is:
# Data
#   - Raw data
#       - experiment name 1
#           - Measurement 1
#               - Data file 1
#               - Data file 2
#                   ...
#           - Measurement 2
#               ...
#           - description file
#       - experiment name 2
#   - Proc data
#       - experiment name 1
#           - graphs
#           - Measurement 1
#               - graph folder
#                   - graphs
#               ...
#           ...
#       - experiment name 2

# Get measurement names for a given experiment
def get_meas(path_to_experiment):
    return get_meas_names_with(path_to_experiment)

# Get measurement names for a given experiment containing phrase "contains"
def get_meas_names_with(path_to_experiment, contains = ""):
    meas = os.listdir(path_to_experiment)
    # get rid of any files found...
    meas = [a for a in meas if os.path.isdir(os.path.join(path_to_experiment, a))]

    if len(contains) > 0:
        meas = [a for a in meas if a.find(contains) != -1]

    return meas

def get_meas_names_ending_with(path_to_experiment, ends_with = ""):
    meas = os.listdir(path_to_experiment)
    # get rid of any files found...
    meas = [a for a in meas if os.path.isdir(os.path.join(path_to_experiment, a))]

    if len(ends_with) > 0:
        meas = [a for a in meas if a.ends_with(ends_with)]

    return meas

def get_data_with(path_to_measurement, contains=""):
    data = os.listdir(path_to_measurement)
    if len(contains) > 0:
        data = [a for a in data if a.find(contains) != -1]

    return data

def get_data_paths(
        path_to_experiment, 
        meas_name_cont = "", 
        data_name_containing = ""):

    result = []
    measurements = get_meas_names_with(path_to_experiment, meas_name_cont);
    for meas in measurements:
        meas_path = os.path.join(path_to_experiment, meas)
        meas_data_files = get_data_with(meas_path, data_name_containing);
        meas_paths = [os.path.join(meas_path, a) for a in meas_data_files]

        result.append(
            {
                'meas_name': meas,
                'meas_data_files': meas_data_files,
                'meas_data_files_paths': meas_paths
            }
        )

    return result
