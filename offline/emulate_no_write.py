import emulate
import json
import sys

class StdOnlyLogger:
    def start(self):
        self.show('info', 'scores will be logged into stdout.')

    def end(self):
        pass

    def show(self, name, data):
        data_json = json.dumps(data, separators=(',', ':'))
        print('{}: {}'.format(name, data_json), file=sys.stderr)

    def info(self, name, data):
        if name == 'scores':
            print(json.dumps(data, separators=(',', ':')))

    def turn(self, turn, data):
        pass

    def warn(self, turn_or_name, data):
        pass

if __name__ == '__main__':
    import log_to_dir
    emulate.emulate(
        emulate.parse_punter_args(at=2),
        emulate.parse_map_arg(at=1),
        StdOnlyLogger())
