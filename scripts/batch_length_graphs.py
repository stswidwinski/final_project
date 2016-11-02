import matplotlib.pyplot as plt
import matplotlib.cm as cm
import os
import pandas
import numpy as np

def aggregate_data(path_to_raw_data):
    # from all the subdirectories grab the necessary pieces of data ...
    dirs = os.listdir(path_to_raw_data)
    relevant_dirs = [a for a in dirs if a.find('batch_length') != -1]

    comp_time_data = pandas.DataFrame(
            {
                'batch_length':[], 
                'avg_comp_time':[], 
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
    for dir in relevant_dirs:
        base_dir = os.path.join(path_to_raw_data, dir);
        batch_length = int(dir[dir.rfind('_') + 1 : ])

        # get the batched completion times
        local_dirs = os.listdir(os.path.join(path_to_raw_data, dir))
        bastd = [a for a in local_dirs if a.find('batched_avg_std') != -1]
        if len(bastd) != 0:
            batched_comp_times = pandas.read_csv(os.path.join(base_dir, bastd[0]));
            comp_time_data = comp_time_data.append(
                    {
                        'batch_length': batch_length, 
                        'avg_comp_time': np.mean(batched_comp_times['total_time']),
                        'model': "batched",
                    }, ignore_index = True)
            avg_txn_comp_data = avg_txn_comp_data.append(
                    {
                        'batch_length': batch_length,
                        'avg_txns_comp_time': np.mean(
                            batched_comp_times['average_completion_time']),
                        'avg_std_dev': np.mean(
                            batched_comp_times['standard_deviation']),
                        'model': "batched",
                    }, ignore_index = True)
        else:
            print("Error creating batch length graphs... files not found.")

        #get the non-batched data points...
        other_astd = [
                a for a in local_dirs if a.find('avg_std') != -1 and a.find('batched') == -1]
        for other_dir in other_astd:
            model = other_dir[:other_dir.find('_avg_std')]
            data = pandas.read_csv(os.path.join(base_dir, other_dir))
            comp_time_data = comp_time_data.append(
                {
                    'batch_length': batch_length,
                    'avg_comp_time': np.mean(data['total_time']),
                    'model': model
                }, ignore_index = True)
            avg_txn_comp_data = avg_txn_comp_data.append(
                {
                    'batch_length': batch_length,
                    'avg_txns_comp_time': np.mean(data['average_completion_time']),
                    'avg_std_dev': np.mean(data['standard_deviation']),
                    'model': model
                }, ignore_index = True)

    return comp_time_data, avg_txn_comp_data
            
def create(path_to_raw_data, path_to_proc_data):
    comp_time, avg_txn = aggregate_data(path_to_raw_data)

    # Create the completion time graphs first
    ax = plt.figure().add_subplot(111)
    plt.title("Workload completion time as a function of batch length")
    plt.xlabel("Batch length")
    plt.ylabel("Completion time")

    models = comp_time['model'].unique().tolist()
    colors = cm.rainbow(np.linspace(0, 1, len(models)))
    for model, color in zip(models, colors):
        ax.scatter(
            comp_time[comp_time['model'] == model]["batch_length"].tolist(),
            comp_time[comp_time['model'] == model]["avg_comp_time"].tolist(),
            label = model,
            color = color)
    plt.legend()
    plt.savefig(os.path.join(path_to_proc_data, 'completion_time_graph'))
    plt.cla()
    plt.clf()

    # Create the average transaction length graph
    ax = plt.figure(0).add_subplot(111)
    plt.title("Average transaction completion time as a function of batch length")
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

