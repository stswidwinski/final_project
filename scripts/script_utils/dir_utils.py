import os

def get_top_proj_level_dir():
    path = os.path.dirname(os.path.realpath(__file__))
    return path[:, path.rfind("scheduling") + len("scheduling")]
