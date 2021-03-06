import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os
import pandas
from script_utils.dir_utils import *
import script_utils.print as p
import numpy as np

def aggregate_data(path_to_raw_data):
    # from all the subdirectories grab the necessary pieces of data ...
    rel_data = get_data_paths(path_to_raw_data, 'batch_length', 'avg_std')

    comp_time_data = pandas.DataFrame(
            {
                'batch_length':[], 
                'total_time':[], 
                'model':[]
            });
    avg_txn_comp_data = pandas.DataFrame(
            {
                'batch_length':[], 
                'avg_txns_comp_time':[], 
                'avg_std_dev':[], 
                'model': []
            })
    # aggregate the data...
    for meas_data in rel_data:
        p.put(meas_data['meas_name']);
        meas_dir = os.path.join(path_to_raw_data, meas_data['meas_name']);
        batch_length = int(meas_dir[meas_dir.rfind('_') + 1 : ])

        for data_file_path in meas_data['meas_data_files_paths']:
            model = data_file_path[
                    data_file_path.rfind('/') + 1 : 
                    data_file_path.rfind('_avg_std')]
            data = pandas.read_csv(data_file_path)
            comp_time_data = comp_time_data.append(
                {
                    'batch_length': batch_length,
                    'total_time': np.mean(data['total_time']),
                    'model': model
                }, ignore_index = True)
            avg_txn_comp_data = avg_txn_comp_data.append(
                {
                    'batch_length': batch_length,
                    'avg_txns_comp_time': np.mean(data['average_completion_time']),
                    'avg_std_dev': np.mean(data['standard_deviation']),
                    'model': model
                }, ignore_index = True)
        p.put(" [ OK ]\n", color="green")

    return comp_time_data, avg_txn_comp_data
            
def create(path_to_raw_data, path_to_proc_data):
    p.put("Aggregating data")
    p.begin_print_section()
    comp_time, avg_txn = aggregate_data(path_to_raw_data)
    p.wipe_section()
    p.put(" [ OK]\n", color="green")

    # Create the completion time graphs first
    ax = plt.figure().add_subplot(111)
    plt.title("Workload completion time as a function of batch length", y = 1.03)
    plt.xlabel("Batch length")
    plt.ylabel("Completion time")

    models = comp_time['model'].unique().tolist()
    colors = cm.rainbow(np.linspace(0, 1, len(models)))
    for model, color in zip(models, colors):
        ax.scatter(
            comp_time[comp_time['model'] == model]["batch_length"].tolist(),
            comp_time[comp_time['model'] == model]["total_time"].tolist(),
            label = model,
            color = color)
    plt.legend()
    plt.savefig(os.path.join(path_to_proc_data, 'completion_time_graph'))
    plt.cla()
    plt.clf()

    # Create the average transaction length graph
    ax = plt.figure(0).add_subplot(111)
    plt.title("Average transaction completion time as a function of batch length", y = 1.03)
    plt.xlabel("Batch length")
    plt.ylabel("Avg completion time")

    models = avg_txn['model'].unique().tolist()
    colors = cm.rainbow(np.linspace(0, 1, len(models)))
    for model, color in zip(models, colors):
        if model == 'sequential':
            continue
        ax.errorbar(
            avg_txn[avg_txn['model'] == model]['batch_length'].tolist(),
            avg_txn[avg_txn['model'] == model]['avg_txns_comp_time'].tolist(),
            yerr = avg_txn[avg_txn['model'] == model]['avg_std_dev'].tolist(),
            label = model,
            color = color)
    plt.legend()
    plt.savefig(os.path.join(path_to_proc_data, 'avg_txn_completion_graph'))
    plt.cla()
    plt.clf()

