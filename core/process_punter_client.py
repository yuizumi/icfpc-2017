from punter_client import PunterClient

class ProcessPunterClient(PunterClient):
    def __init__(self, command):
        self.command = command

    def start_handshake(self):
        raise "todo"

    def end_handshake(self, input_dict):
        raise "todo"

    def setup(self, input_dict):
        return move(input_dict)

    def play(self, input_dict):
        raise "todo"
        
    def stop(self, input_dict):
        return move(input_dict)
