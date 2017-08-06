import numpy as np
from scipy.sparse import csgraph

class PunterState:
    def __init__(self, client):
        self.client = client
        self.name = None
        self.id = None
        self.state = None
        self.moves = []
        self.last_move = None

def edges_equal(a, b):
    return ((a['source'] == b['source'] and a['target'] == b['target'])
        or (a['source'] == b['target'] and a['target'] == b['source']))

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
            'punters': len(punters),
            'map': map_dict })
        if 'state' in ready:
            punter.state = ready['state']
        logger.show('punter '+ str(punter.id), punter.name)
    logger.info('punters', [{'punter': p.id, 'name': p.name} for p in punters])

    # game
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
        if 'claim' in move:
            claim = move['claim']
            assert claim['punter'] == punter.id
            rivers = [r for r in map_dict['rivers']
                      if edges_equal(r, claim) and 'punter' not in r]
            if len(rivers) == 1:
                rivers[0]['punter'] = punter.id
            else:
                logger.warn(turn, move)
                move = None
        elif 'pass' not in move:
            logger.warn(turn, move)
            move = None
        # error json is regarded as pass
        if move is None:
            logger.show('suspicious move', {'turn': turn, 'name': punter.name})
            move = {'pass': {'punter': punter.id}, 'state': punter.state}
        logger.turn(turn, move)
        if 'state' in move: del move['state']
        logger.show(turn, move)
        punter.moves.append(move)
        all_moves.append(move)
        punter.last_move = move
        punter_index = (punter_index + 1) % len(punters)

    # calc score
    site_count = len(map_dict['sites'])
    site_to_index = {}
    for i in range(site_count):
        site_to_index[map_dict['sites'][i]['id']] = i
    graph = np.zeros([site_count] * 2, np.bool)
    for r in map_dict['rivers']:
        s = site_to_index[r['source']]
        t = site_to_index[r['target']]
        graph[s, t] = True
        graph[t, s] = True
    distance = csgraph.floyd_warshall(graph, False, False, True)
    for punter in punters:
        subgraph = np.zeros_like(graph)
        for m in punter.moves:
            if 'claim' not in m: continue
            s = site_to_index[m['claim']['source']]
            t = site_to_index[m['claim']['target']]
            subgraph[s, t] = True
            subgraph[t, s] = True
        subdist = csgraph.floyd_warshall(subgraph, False, False, True)
        punter.score = 0
        for mine in map_dict['mines']:
            mine_index = site_to_index[mine]
            for i in range(site_count):
                if not np.isinf(subdist[mine_index, i]):
                    punter.score += int(distance[mine_index, i]) ** 2

    # send score
    moves = [p.last_move for p in punters]
    scores = [{'punter': p.id, 'score': p.score} for p in punters]
    for punter in punters:
        hand_req = punter.client.start_handshake()
        assert hand_req['me'] == punter.name
        punter.client.end_handshake({'you': punter.name})
        punter.client.stop({
            'stop': {'moves': moves, 'scores': scores},
            'state': punter.state})

    # log
    logger.info('moves_summary', all_moves)
    logger.info('map_result', map_dict)
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
