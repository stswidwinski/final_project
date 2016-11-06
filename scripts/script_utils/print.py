import sys

colors = {
    'green': '\033[1;32;40m ',
    'red': '\033[1;31;40m ',
    'white': '\033[1;37;40m '
};

def put(text, color='white'):
    sys.stdout.write(colors[color] + text + '\033[0m');
    sys.stdout.flush()
