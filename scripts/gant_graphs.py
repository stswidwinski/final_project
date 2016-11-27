#!/usr/bin/python3

import matplotlib.pyplot as plt
import os
import pandas
import numpy as np
from script_utils.dir_utils import *
import script_utils.print as p

def create(path_to_raw_data, path_to_proc_data, batch_length = 0):
    all_data = get_data_paths(path_to_raw_data, "", "gant");

    for meas_data in all_data:
        if batch_length == 0:
            batch_length = int(meas_data['meas_name'][meas_data['meas_name'].rfind('_') + 1 :]);
        p.put("Creating gant plot for " + meas_data['meas_name'] + "\n");
        save_to = os.path.join(path_to_proc_data, meas_data['meas_name'], "gant_graphs");
        create_dir(save_to);
        p.begin_print_section()
        for data_file_path in meas_data['meas_data_files_paths']:
            p.put("Reading data for " + data_file_path)
            data = pandas.read_csv(data_file_path)
            p.put(" [ OK ]\n", color="green")
            p.put("Creating graph")
            plt.suptitle("Gantt chart of transactions")
            ax = plt.subplot(111)
            # Prepare the data...
            begs = []
            arr_len = []
            arr_left = []
            arr_colors = []
            start_len = []
            start_left = []
            for i in range(0, len(data.index)):
                begs.append(data['txn_id'][i])
                arr_len.append(data['duration'][i] + data['start_time'][i] - data['arrival_time'][i])
                arr_left.append(data['arrival_time'][i])
                start_len.append(data['duration'][i])
                start_left.append(data['start_time'][i])
                if (data['excl_locks'][i] == 0):
                    arr_colors.append('g')
                else: 
                    arr_colors.append('r')
            
            # Actually create the graphs
            ax.barh(
                    np.array(begs)[np.array(arr_colors) == 'g'], 
                    np.array(arr_len)[np.array(arr_colors) == 'g'], 
                    height = 1.0, 
                    left = np.array(arr_left)[np.array(arr_colors) == 'g'], color = 'g', label = "Time in system (read txn)")
            ax.barh(
                    np.array(begs)[np.array(arr_colors) == 'r'], 
                    np.array(arr_len)[np.array(arr_colors) == 'r'], 
                    height = 1.0, 
                    left = np.array(arr_left)[np.array(arr_colors) == 'r'], color = 'r', label = "Time in system (write txn)")
            ax.barh(begs, start_len, height = 0.35, left = start_left, color = 'b', label = "Execution time")

            # Add markers for beginning of the batches
            for i in range(batch_length, np.max(data['arrival_time']) + batch_length + 1, batch_length):
                ax.axvline(x = i, ls = 'dashed')
            ax.set_yticks(range(0, np.max(data['txn_id']) + 1, 2))
            ax.set_xlabel('Time')
            ax.set_ylabel('Transaction Id')

            # Shrink the graph and place the legen below it.
            box = ax.get_position()
            ax.set_position([box.x0, box.y0 + box.height*0.1, box.width, box.height * 0.9])
            ax.legend(loc = 'upper center', bbox_to_anchor=(0.5, -0.05), fancybox=True, shadow=True, ncol = 2)

            # save it
            path_to_save = os.path.join(
                    save_to,
                    data_file_path[data_file_path.rfind('/') + 1:])
            plt.savefig(path_to_save)
            p.put(" [ OK ]\n", color="green")
            plt.cla()
            plt.clf()
        p.wipe_section()
        p.put("Creating gant plot for " + meas_data['meas_name'])
        p.put (" [ OK ]\n", color="green")
