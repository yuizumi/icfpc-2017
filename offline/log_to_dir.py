import json
import os
import sys

class DirLogger:
    def __init__(self, root_dir):
        self.root_dir = root_dir

    def start(self):
        try:
            os.mkdir(self.root_dir)
            self.show('mkdir', self.root_dir)
        except FileExistsError:
            pass

        index = 0
        for name in os.listdir(self.root_dir):
            try:
                index = max(index, int(name))
            except ValueError:
                pass

        self.dir = os.path.join(self.root_dir, str(index + 1))
        try:
            os.mkdir(self.dir)
            self.show('mkdir', self.dir)
        except FileExistsError:
            pass
        self.show('logging to', self.dir)

    def end(self):
        self.show('logged to', self.dir)

    def show(self, name, data):
        data_json = json.dumps(data, separators=(',', ':'))
        print('{}: {}'.format(name, data_json), file=sys.stderr)

    def info(self, name, data):
        file_path = os.path.join(self.dir, name + '.json')
        with open(file_path, 'w') as file:
            json.dump(data, file, separators=(',', ':'))

    def turn(self, turn, data):
        self.info(str(turn), data)

    def warn(self, turn_or_name, data):
        self.info('warn-' + str(turn_or_name), data)


def default_logger():
    dir = os.path.dirname(__file__)
    return DirLogger(os.path.join(dir, '../visualizer/log'))
