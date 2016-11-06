import argparse


def init_parser(parser):
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
    add_argument(
            '-bsc', 
            "Bursty transaction generation seed chance (the smaller, the more bursty)")
    add_argument('-blf', "Bursty linear factor. The higher, the less bursty")
    add_argument('-bl', "Batch length", True)
    add_argument('-en', "Experiment name")
    parser.add_argument(
            '-reps', 
            help="Repetitions of each measurement", 
            required=False, 
            default="3")
    parser.add_argument(
            '-mods', 
            help="Model names", 
            nargs="+", 
            required=True, 
            choices=["sequential", "real_time", "batched"]);
    parser.add_argument(
            '-data', 
            help="Data to gather", 
            nargs="+", 
            required=True, 
            choices=["load", "avg_proc_time", "std_dev_proc_time", "locks_in_time"]);
    
