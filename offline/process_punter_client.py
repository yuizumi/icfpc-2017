import io
import json
import subprocess

class ProcessPunterClient:
    def __init__(self, command):
        self.command = command
        self.reader = None
        self.writer = None
        self.process = None

    def read(self):
        assert self.reader is not None
        length = ''
        while True:
            ch = self.reader.read(1)
            if ch == ':': break
            length += ch
        data = self.reader.read(int(length))
        return json.loads(data)

    def write(self, dict):
        assert self.process is not None
        data = json.dumps(dict)
        self.writer.write(str(len(data)))
        self.writer.write(':')
        self.writer.write(data)
        self.writer.flush()

    def start_handshake(self):
        if self.process is not None:
            self.writer.close()
            self.reader.close()
            self.process.wait()
        self.process = subprocess.Popen(
            self.command,
            shell=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
        self.reader = io.TextIOWrapper(self.process.stdout, encoding='utf8')
        self.writer = io.TextIOWrapper(self.process.stdin, encoding='utf8')
        return self.read()

    def end_handshake(self, input_dict):
        self.write(input_dict)

    def setup(self, input_dict):
        self.write(input_dict)
        return self.read()

    def play(self, input_dict):
        self.write(input_dict)
        return self.read()
        
    def stop(self, input_dict):
        self.write(input_dict)
