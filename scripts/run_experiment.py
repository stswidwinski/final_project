import argparse
import subprocess
import os
from script_utils.dir_utils import *
from enum import Enum
import load_graphs
import batch_length_graphs
import lock_data_graphs

parser = argparse.ArgumentParser(
        description='Run an experiment with given params and make graphs')

def add_argument(arg_tag, help_string, plural=False, parser=parser):
    if plural:
        parser.add_argument(arg_tag, help=help_string, nargs="+", required=True)
    else:
        parser.add_argument(arg_tag, help=help_string, required=True)

add_argument(
        '-ltm', 
        "Linear time multiplier. The length of a transaction is the number of locks it has *ltm")
add_argument('-tp', "Time period. The time is a linear counter 0 through time period - 1.")
add_argument('-tn', "Number of transactions in the simulation")
add_argument('-ulss', "Uncontested Lock Space Size")
add_argument('-ulavg', "Average number of unconstedted locks held by txn", True)
add_argument(
        '-ulstd', 
        "Standard deviation of the average # of uncontested locks held by txn")
add_argument('-clss', "Contested Lock Space Size")
add_argument('-clavg', "Average number of constedted locks held by txn", True)
add_argument(
        '-clstd', 
        "Standard deviation of the average # of contested locks held by txn")
add_argument('-wtxn', "Percentage of write transactions", True)
add_argument('-bsc', "Bursty transaction generation seed chance (the smaller, the more bursty)")
add_argument('-blf', "Bursty linear factor. The higher, the less bursty")
add_argument('-bl', "Batch length", True)
add_argument('-en', "Experiment name")
parser.add_argument('-mods', help="Model names", nargs="+", required=True, choices=["sequential", "real_time", "batched"]);
parser.add_argument('-data', help="Data to gather", nargs="+", required=True, choices=["load", "avg_proc_time", "std_dev_proc_time", "locks_in_time"]);
args = parser.parse_args()

# Check that the parameters passed in are legal.
if (len(args.ulavg) == 0 and len(args.ulavg) == 0):
    print("Locks must exist in the system")
    exit()

# create necessary dirs.
raw_data_dir = os.path.join(get_raw_data_dir(), args.en)
create_dir(raw_data_dir)
print("Created directory " + raw_data_dir + " for output")
proc_data_dir = os.path.join(get_proc_data_dir(), args.en)
create_dir(proc_data_dir)
print("Created directory " + proc_data_dir + " for graphs")

# run all the specified experiments...
#
# We allow only few types of experiments:
#   1) Multiple batch lengths for static else
#   2) Different write txn percentages for static else
#   3) Different contested/uncontested percentages for static else
#
#   The first plural argument specifies the experiment.
class Experiment(Enum):
    batch_length = 1
    write_txn_perc = 2
    contested = 3
    uncontested = 4

experiment_type = ''
iterate_over = ''
if (len(args.ulavg) > 1):
    experiment_type = Experiment.uncontested
    iterate_over = args.ulavg
elif (len(args.clavg) > 1):
    experiment_type = Experiment.contested
    iterate_over = args.clavg
elif (len(args.wtxn) > 1):
    experiment_type = Experiment.write_txn_perc
    iterate_over = args.wtxn
else:
    # default
    experiment_type = Experiment.batch_length
    iterate_over = args.bl

path_suffixes = []
for i in range(0, len(iterate_over)):
    # For every element of the experiment we create a separate directory
    file_suffix = experiment_type.name + "_" + str(iterate_over[i])
    raw_file_path = os.path.join(raw_data_dir, file_suffix)
    create_dir(raw_file_path)

    cmd = [os.path.join(get_top_proj_level_dir(), "bin/simulation/simulation_test")]
    cmd.append(args.ltm)
    cmd.append(args.tp)
    cmd.append(args.tn)
    
    cmd.append(args.ulss)
    if experiment_type != Experiment.uncontested:
        cmd.append(args.ulavg[0])
    else:
        cmd.append(args.ulavg[i])
    cmd.append(args.ulstd)
    
    cmd.append(args.clss)
    if experiment_type != Experiment.contested:
        cmd.append(args.clavg[0])
    else:
        cmd.append(args.clavg[i])
    cmd.append(args.clstd)

    if experiment_type != Experiment.write_txn_perc:
        cmd.append(args.wtxn[0])
    else:
        cmd.append(args.wtxn[i])

    cmd.append(args.bsc)
    cmd.append(args.blf)
    if experiment_type != Experiment.batch_length:
        cmd.append(args.bl[0])
    else:
        cmd.append(args.bl[i])

    cmd.append(raw_file_path)
    for a in args.mods:
        cmd.append(a)
    
    for a in args.data:
        cmd.append(a)

    cmd_string = ""
    for a in cmd:
        cmd_string += " " + a

    print("Executing " + cmd_string)
    subprocess.call(cmd)

# Create the graphs for all the experiments. Again, this depends on the
# experiment conducted...
if 'load' in args.data:
    print("Creating load graps...")
    load_graphs.create(raw_data_dir, proc_data_dir, 1000)

if experiment_type == Experiment.batch_length:
    if 'avg_proc_time' in args.data or 'std_dev_proc_time' in args.data:
        batch_length_graphs.create(raw_data_dir, proc_data_dir)    
    if 'locks_in_time' in args.data:
        lock_data_graphs.create(raw_data_dir, proc_data_dir) 
