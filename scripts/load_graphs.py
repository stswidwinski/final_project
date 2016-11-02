#!/usr/bin/python3

import matplotlib.pyplot as plt
import os
import pandas
from script_utils.dir_utils import *

def get_start_time_histo(start_times, bins):
    plt.hist(start_times, bins = bins)
    plt.gca().set_title("Txns by start time")
    plt.xlabel("Start Time")
    plt.ylabel("Txn Number")

def get_end_time_histo(start_times, duration, bins):
    plt.hist(start_times + duration, bins = bins)
    plt.gca().set_title("Txns by end time")
    plt.xlabel("End Time")
    plt.ylabel("Txn Number")

def get_paths(path_to_raw_data):
    path = os.path.join(path_to_raw_data, "descr")
    description = pandas.read_csv(path)
    description = description[description['data_type'] == 'load']
    return description['path_to_result'].tolist()

def create(path_to_raw_data, path_to_proc_data, bins):
    all_dirs = os.listdir(path_to_raw_data)
    for dir in all_dirs:
        dir_path = os.path.join(path_to_raw_data, dir)
        all_paths = get_paths(dir_path)
        save_to = os.path.join(path_to_proc_data, dir, "load_graphs")
        create_dir(save_to)
        for path in all_paths:
            data = pandas.read_csv(path);
            plt.suptitle("Load data")
            plt.subplot(211)
            get_start_time_histo(data['start_time'], bins)
            plt.subplot(212)
            get_end_time_histo(data['start_time'], data['duration'], bins)
            plt.subplots_adjust(top=0.85)
            plt.tight_layout()
            path_to_save = os.path.join(
                    save_to,
                    path[path.rfind('/') + 1:])
            plt.savefig(path_to_save)
            plt.clf()
            plt.cla()
