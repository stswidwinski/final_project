import os

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

    raise NameError('Directory ' + directory + 'already exists!')
