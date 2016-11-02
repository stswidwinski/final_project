import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os
import pandas
import numpy as np
from script_utils.dir_utils import *

def aggregate_data_for_directory(path):
    # For every lock data file in the dir, make a plot
    all_files = os.listdir(path)
    relevant_files = [a for a in all_files if a.find('locks_in_time') != -1]

    # Find the maximal index on any of those files
    max_index = 0
    for file in relevant_files:
        index = int(file[file.rfind('_') + 1:])
        if index > max_index:
             max_index = index
    
    # Create pandas data frames for all of these
    dfs = []
    for index in range(0, max_index + 1):
        wanted_files = [a for a in relevant_files if a.endswith('_' + str(index))]
        print(wanted_files) 
        # each file corresponds to a different model
        series = []
        for file in wanted_files:
            model = file[:file.find("_locks_in_time")]
            file_path = os.path.join(path, file)
            data = pandas.read_csv(file_path)
            series.append([model, data['number_of_locks']])
        
        data_frame = pandas.DataFrame()
        for serie in series:
            data_frame[serie[0]] = serie[1]
        
        dfs.append(
            { 
                'index': index,
                'df': data_frame
            })

    return dfs

def create(path_to_raw_data, path_to_proc_data):
    # make the graphs!
    all_dirs = os.listdir(path_to_raw_data)
    for dir in all_dirs:
        save_path = os.path.join(path_to_proc_data, dir, "lock_graphs")
        create_dir(save_path)

        dir_path = os.path.join(path_to_raw_data, dir);
        data = aggregate_data_for_directory(dir_path)
        for df_elt in data:
            ax = plt.figure(0).add_subplot(111)
            plt.title("Concurrent txns in time")
            plt.xlabel("Time")
            plt.ylabel("Number of transactions in flight")

            columns = df_elt['df'].columns.tolist()
            print(columns)
            colors = cm.rainbow(np.linspace(0, 1, len(columns)))
            for column, color in zip(columns, colors):
                data = df_elt['df'][column].dropna()
                ax.scatter(
                        data.index.tolist(),
                        data.tolist(),
                        label = column,
                        color = color)
            plt.legend()
            plt.savefig(os.path.join(save_path, "concur_txns_" + str(df_elt['index'])))
            plt.cla()
            plt.clf()
