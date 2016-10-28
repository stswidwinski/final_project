#!/usr/bin/python3

import matplotlib.pyplot as plt
import pandas

BINS = 1000

def get_start_time_histo(start_times):
    plt.hist(start_times, bins = BINS)
    plt.gca().set_title("Txns by start time")
    plt.xlabel("Start Time")
    plt.ylabel("Txn Number")

def get_end_time_histo(start_times, duration):
    plt.hist(start_times + duration, bins = BINS)
    plt.gca().set_title("Txns by end time")
    plt.xlabel("End Time")
    plt.ylabel("Txn Number")

def create(path_to_data, path_to_save):
    data = pandas.read_csv(path_to_data);
#    plt.tight_layout()
    plt.suptitle("Load data")
    plt.subplot(211)
    get_start_time_histo(data['start_time'])
    plt.subplot(212)
    get_end_time_histo(data['start_time'], data['duration'])
    plt.subplots_adjust(top=0.85)
    plt.savefig(path_to_save)
    plt.clf()
    plt.cla()
