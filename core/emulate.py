import numpy as np
import sys
from punter_client import PunterClient
from process_punter_client import ProcessPunterClient
from scipy.sparse import csgraph

class PunterState:
    def __init__(self, client):
        self.client = client
        self.name = None
        self.id = None
        self.state = None
        self.moves = []
        self.last_move = None

def emulate(punter_clients, map_dict):
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
        if 'state' in move:
            punter.state = move['state']
        # validate and save move
        if 'claim' in move:
            claim = move['claim']
            assert claim['punter'] == punter.id
            rivers = [r for r in map_dict['rivers']
                      if r['source'] == claim['source']
                      and r['target'] == claim['target']
                      and 'punter' not in r]
            if len(rivers) == 1:
                rivers[0]['punter'] = punter.id
            else:
                move = {'pass': {'punter': punter.id}}
        else:
            move = {'pass': {'punter': punter.id}}
        if 'state' in move: del move['state']
        print(move)
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
    print(map_dict)
    print(scores)

# TODO: use argparse
emulate(
    [ProcessPunterClient(sys.argv[1]), PunterClient()],
    {'sites': [{'id': 1}, {'id': 2}, {'id': 3}, {'id': 4}],
     'rivers': [
         {'source': 1, 'target': 2},
         {'source': 2, 'target': 3},
         {'source': 3, 'target': 4},
         {'source': 1, 'target': 4}],
     'mines': [1, 3]})
