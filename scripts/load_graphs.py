#!/usr/bin/python3

import matplotlib.pyplot as plt
import os
import pandas
from script_utils.dir_utils import *
from script_utils.print import *

def get_arrival_time_histo(arrival_times, bins):
    plt.hist(arrival_times, bins = bins)
    plt.gca().set_title("Txns by start time")
    plt.xlabel("Arrival Time")
    plt.ylabel("Txn Number")

def get_end_time_histo(arrival_times, duration, bins):
    plt.hist(arrival_times + duration, bins = bins)
    plt.gca().set_title("Txns by end time")
    plt.xlabel("End Time")
    plt.ylabel("Txn Number")

def create(path_to_raw_data, path_to_proc_data, bins):
    all_data = get_data_paths(path_to_raw_data, "", "load")

    for meas_data in all_data:
        begin_print_section()
        put("Creating plot for " + meas_data['meas_name'] + "\n")
        save_to = os.path.join(path_to_proc_data, meas_data['meas_name'], "load_graphs")
        create_dir(save_to)
        for data_file_path in meas_data['meas_data_files_paths']:
            put("Reading data for " + data_file_path)
            data = pandas.read_csv(data_file_path);
            put(" [ OK ]\n", color="green")
            put("Creating graph")
            plt.suptitle("Load data", y = 1.02)
            plt.subplot(111)
            get_arrival_time_histo(data['arrival_time'], bins)
            path_to_save = os.path.join(
                    save_to, 
                    data_file_path[data_file_path.rfind('/') + 1:])
            plt.savefig(path_to_save)
            put(" [ OK ]\n", color="green")
            plt.clf()
            plt.cla()
        wipe_section()
        put("Creating plot for " + meas_data['meas_name'])
        put(" [ OK ]\n", color="green")
