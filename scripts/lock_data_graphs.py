import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os
import pandas
import numpy as np
from script_utils.dir_utils import *

class LockDataRepetition:
    index = 0
    data_file_paths = []
    
    def __init__(self):
        self.index = 0
        self.data_file_paths = []

class MeasLockData:
    name = ''
    # an array of lock data repetitions
    reps = []

    def __init__(self):
        self.name = ''
        self.reps = []

def aggregate_metadata(raw_data_path):
    all_data_file_paths = get_data_paths(raw_data_path, "", "txns_in_time");

    # An array of MeasLockData.
    data_metadata = []
    for meas_data in all_data_file_paths:
        meas_metadata = MeasLockData()
        meas_metadata.name = meas_data['meas_name']
        
        meas_files = meas_data['meas_data_files_paths']
        max_index = 0
        for path in meas_files:
            index = int(path[path.rfind('_') + 1:])
            if index > max_index:
                max_index = index

        for index in range(0, max_index + 1):
            rep_meta = LockDataRepetition()
            rep_meta.data_file_paths = [a for a in meas_files if a.endswith('_' + str(index))]
            rep_meta.index = index
            meas_metadata.reps.append(rep_meta)
        data_metadata.append(meas_metadata)
    return data_metadata

def get_rep_data(meas_data_path, rep_meta_data):
    series = []
    for rep_file_path in rep_meta_data.data_file_paths:
        # each rep is a different model
        model = rep_file_path[
            rep_file_path.rfind('/') + 1 :
            rep_file_path.rfind("_txns_in_time")]
        local_data = pandas.read_csv(rep_file_path)
        series.append([model, local_data['number_of_txns']])
       
    # sort longest to shortest to overcome limitations of pandas.insert
    series = sorted(series, key=lambda serie: len(serie[1]), reverse=True)
    data_frame = pandas.DataFrame()
    for serie in series:
        data_frame.insert(len(data_frame.columns.tolist()), serie[0], serie[1])

    return data_frame    

def create(path_to_raw_data, path_to_proc_data):
    # make the graphs!
    metadata = aggregate_metadata(path_to_raw_data)
    for mdata in metadata:
        # We are considering the MeasLockData elements
        save_path = os.path.join(path_to_proc_data, mdata.name, "lock_graphs")
        create_dir(save_path)
        meas_path = os.path.join(path_to_raw_data, mdata.name)

        for rep in mdata.reps:
            rep_data = get_rep_data(meas_path, rep)
            
            # scatter plot
            ax = plt.figure(0).add_subplot(111)
            plt.title("Concurrent txns in time")
            plt.xlabel("Time")
            plt.ylabel("Number of transactions in flight")

            columns = sorted(rep_data.columns.tolist(), key=lambda column: len(rep_data[column].dropna().tolist()), reverse=True)
            colors = cm.rainbow(np.linspace(0, 1, len(columns)))
            for column, color in zip(columns, colors):
                data = rep_data[column].dropna()
                ax.scatter(
                        data.index.tolist(),
                        data.tolist(),
                        label = column,
                        color = color)
            plt.legend()
            plt.savefig(os.path.join(save_path, "concur_txns_" + str(rep.index)))
            plt.cla()
            plt.clf()

            # histogram plot
            ax = plt.figure(0).add_subplot(111)
            plt.title("Concurrent txns during execution")
            plt.xlabel("Concurrent txns")
            plt.ylabel("Aggregated time")

            colors = cm.rainbow(np.linspace(0, 1, len(columns)))
            for column, color in zip(columns, colors):
                data = rep_data[column].dropna()
                ax.hist(data, 100, alpha=0.5, label=column)
            plt.legend()
            plt.savefig(os.path.join(save_path, "concur_txns_hist_" + str(rep.index)))
            plt.cla()
            plt.clf()
