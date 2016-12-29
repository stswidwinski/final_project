import argparse
import subprocess
import os
import sys
import script_utils.dir_utils as dir_utils
import script_utils.description_file as description_file
import script_utils.args as args_parsing
import script_utils.print as p
from script_utils.dir_utils import *
from enum import Enum
import load_graphs
import batch_length_graphs
import lock_data_graphs
import gant_graphs
import dep_graphs

import time

def run_experiment():
    # Initialize parser and get arguments
    parser = argparse.ArgumentParser(
            description='Run an experiment with given params and make graphs')
    args_parsing.init_parser(parser)
    args = parser.parse_args()

    # Check that the parameters passed in are legal.
    if (len(args.ulavg) == 0 and len(args.ulavg) == 0):
        print("Locks must exist in the system")
        exit()

    # create necessary dirs.
    raw_data_dir = os.path.join(get_raw_data_dir(), args.en)
    p.put("Creating directory " + raw_data_dir);
    create_dir(raw_data_dir)
    p.put( " [ OK ]\n", "green")

    proc_data_dir = os.path.join(get_proc_data_dir(), args.en)
    p.put("Created directory " + raw_data_dir)
    create_dir(proc_data_dir)
    p.put( " [ OK ]\n", "green")

    # Write txn description file
    description_file.write(proc_data_dir, args)

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
        txn_num = 5

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
    elif (len(args.tn) > 1):
        experiment_type = Experiment.txn_num
        iterate_over = args.tn
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
        if experiment_type != Experiment.txn_num:
            cmd.append(args.tn[0])
        else:
            cmd.append(args.tn[i])
        
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
        cmd.append(args.reps)
        for a in args.mods:
            cmd.append(a)
        
        for a in args.data:
            cmd.append(a)

        cmd_string = ""
        for a in cmd:
            cmd_string += " " + a
       
       # p.put(cmd_string + "\n");
        p.put("Running " + experiment_type.name + " experiment " + str(i + 1) + " out of " + str(len(iterate_over)))
        if subprocess.call(cmd) < 0:
            sys.exit(-1)
    p.put("Running experiments")
    p.put(" [ OK ]\n", color='green')

    # Create the graphs for all the experiments. Again, this depends on the
    # experiment conducted...
    if 'load' in args.data:
        p.begin_print_section();
        p.put("Creating load graphs\n")
        load_graphs.create(raw_data_dir, proc_data_dir, 1000)
        p.wipe_section()
        p.put("Creating load graphs")
        p.put(" [ OK ]\n", color = 'green')

    if experiment_type == Experiment.batch_length:
        if 'avg_proc_time' in args.data or 'std_dev_proc_time' in args.data:
            p.begin_print_section();
            p.put("Creating avg proc time graphs\n")
            batch_length_graphs.create(raw_data_dir, proc_data_dir)    
            p.wipe_section()
            p.put("Creating avg proc time graphs")
            p.put(" [ OK ]\n", color = "green")
        if 'locks_in_time' in args.data:
            p.begin_print_section();
            p.put("Creating txns in time graphs\n")
            lock_data_graphs.create(raw_data_dir, proc_data_dir) 
            p.wipe_section()
            p.put("Creating txns in time graphs")
            p.put(" [ OK ]\n", color = "green")

    if 'txn_gant' in args.data:
        p.begin_print_section();
        p.put("Creating gant graphs\n")
        batch_length = 0
        if (experiment_type != Experiment.batch_length):
            batch_length = int(args.bl[0])
        gant_graphs.create(raw_data_dir, proc_data_dir, batch_length)
        p.wipe_section()
        p.put("Creating gant graphs")
        p.put(" [ OK ]\n", color = "green")

    if 'dep_graph' in args.data:
        p.begin_print_section();
        p.put("Creating dependency graphs\n")
        dep_graphs.create(raw_data_dir, proc_data_dir)
        p.wipe_section()
        p.put("Creating dependency graphs")
        p.put(" [ OK ]\n", color = "green")

if __name__ == '__main__':
    run_experiment()
