import sys

colors = {
    'green': '\033[1;32;40m ',
    'red': '\033[1;31;40m ',
    'white': '\033[1;37;40m '
};

print_section = []

def put(text, color='white'):
    sys.stdout.write(colors[color] + text + '\033[0m');
    sys.stdout.flush()

    if (text.rfind('\n') != -1 and len(print_section) > 0):
        indent()
        print_section[-1] += 1

def indent():
    sys.stdout.write("\r")
    for i in range(0, len(print_section)):
        sys.stdout.write("\t")
    sys.stdout.flush()

def begin_print_section():
    print_section.append(0)
    indent()

def wipe_section():
    global print_section
    while (print_section[-1] > 0):
        sys.stdout.write("\033[2K\033[A\033[2K")
        print_section[-1] -= 1;
    
    sys.stdout.write("\033[A")
    print_section = print_section[:-1]
    sys.stdout.flush()
