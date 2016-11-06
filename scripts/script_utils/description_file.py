import os
import sys
import script_utils.print as p

def write_helper(file, name, args, delim="\t"):
    if len(name) > 0:
        file.write('{:<30}\t\t'.format(name));
    
    for arg in args:
        file.write(arg + delim)
    file.write("\n")

def write(path, args):
    p.put("Writing description file ")
    file_path = os.path.join(path, "description")
    file = open(file_path, "w")

    file.write(args.en + "\n\n")
    file.write("Generation information\n")
    write_helper(file, "Time Period", [args.tp])
    write_helper(file, "Transaction Number", [args.tn])
    write_helper(file, "Linear Time Multiplier", [args.ltm])
    write_helper(file, "Uncontested lock space size", [args.ulss])
    write_helper(file, "Uncontested lock held avg", args.ulavg)
    write_helper(file, "Uncontested lock held std dev", [args.ulstd])
    write_helper(file, "Contested lock space size", [args.clss])
    write_helper(file, "Contested lock held avg", args.clavg)
    write_helper(file, "Contested lock held std dev", [args.clstd]) 
    write_helper(file, "Write Txn Perc", args.wtxn)
    write_helper(file, "Bursty Seed Chance", [args.bsc])
    write_helper(file, "Bursty Linear Factor", [args.blf])

    file.write("\nExperiment information\n\n")
    write_helper(file, "Batch Length", args.bl)
    write_helper(file, "Repetitions", [args.reps])
    write_helper(file, "Models used", args.mods)
    write_helper(file, "Data gathered", args.data)

    file.write("\nCommand To execute\n\n");
    write_helper(file, "", sys.argv, " ");
    file.close()
    p.put(" [ OK ]\n", color="green")
