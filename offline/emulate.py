from score import Score

class PunterState:
    def __init__(self, client):
        self.client = client
        self.name = None
        self.id = None
        self.state = None
        self.moves = []
        self.last_move = None

def emulate(punter_clients, map_dict, logger):
    logger.start()
    logger.info('map', map_dict)
    punters = []

    # setup
    for client in punter_clients:
        punter = PunterState(client)
        punters.append(punter)
        # handshake
        hand_req = punter.client.start_handshake()
        punter.name = hand_req['me']
        punter.id = len(punters) - 1
        punter.last_move = {'pass': {'punter': punter.id}}
        punter.client.end_handshake({'you': punter.name})
        # setup
        ready = punter.client.setup({
            'punter': punter.id,
            'punters': len(punter_clients),
            'map': map_dict })
        if 'state' in ready:
            punter.state = ready['state']
        logger.show('punter '+ str(punter.id), punter.name)
    logger.info('punters', [{'punter': p.id, 'name': p.name} for p in punters])

    # game
    score = Score(map_dict, len(punters))
    punter_index = 0
    all_moves = []
    for turn in range(len(map_dict['rivers'])):
        punter = punters[punter_index]
        hand_req = punter.client.start_handshake()
        assert hand_req['me'] == punter.name
        punter.client.end_handshake({'you': punter.name})
        move = punter.client.play({
            'move': {'moves': [p.last_move for p in punters]},
            'state': punter.state})
        # save state
        punter.state = move.get('state')
        # validate and save move
        valid_move = score.move(move, punter, logger, turn)
        logger.turn(turn, valid_move)
        if 'state' in valid_move: del valid_move['state']
        logger.show(turn, valid_move)
        punter.moves.append(valid_move)
        all_moves.append(valid_move)
        punter.last_move = valid_move
        punter_index = (punter_index + 1) % len(punters)

    # send score
    moves = [p.last_move for p in punters]
    scores = [{'punter': p.id, 'score': score.calc(p.id)} for p in punters]
    for punter in punters:
        hand_req = punter.client.start_handshake()
        assert hand_req['me'] == punter.name
        punter.client.end_handshake({'you': punter.name})
        punter.client.stop({
            'stop': {'moves': moves, 'scores': scores},
            'state': punter.state})

    # log
    logger.info('moves_summary', all_moves)
    logger.info('rivers_result', score.rivers)
    logger.info('scores', scores)
    logger.show('scores', scores)
    logger.end()

def parse_map_arg(at):
    import json
    import sys
    with open(sys.argv[at]) as maps_json:
        return json.load(maps_json)

def parse_punter_args(at):
    import sys
    from process_punter_client import ProcessPunterClient
    clients = []
    for command in sys.argv[2:]:
        clients.append(ProcessPunterClient(command))
    return clients

if __name__ == '__main__':
    import log_to_dir
    emulate(
        parse_punter_args(at=2),
        parse_map_arg(at=1),
        log_to_dir.default_logger())
