import os
import subprocess
import pandas
import numpy as np
from script_utils.dir_utils import *
import script_utils.print as p

def create(path_to_raw_data, path_to_proc_data):
    all_data = get_data_paths(path_to_raw_data, "", "dep_graph")

    for meas_data in all_data:
        p.begin_print_section()
        p.put("Creating dependency graph for " + meas_data['meas_name'] + "\n")
        move_to = os.path.join(path_to_proc_data, meas_data['meas_name'], "dep_graphs")
        create_dir(move_to)
        for data_file_path in meas_data['meas_data_files_paths']:
            p.put("Creating graph for " + data_file_path)
            cmd = ["blockdiag", "--no-transparency",data_file_path]
            subprocess.call(cmd)
            p.put(" [ OK ]\n", color='green')
            # Move it to the other folder
            cmd = ["mv", data_file_path + ".png", move_to]
            subprocess.call(cmd)
        p.wipe_section()
        p.put("Creating dependency plot for " + meas_data['meas_name'])
        p.put(" [ OK ]\n", color='green')
