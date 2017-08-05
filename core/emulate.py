class Punter:
    # emulatorがいじる状態（別クラスにすべきだが）
    name = None
    id = None
    state = None
    last_move = None

    # 以下はテスト用実装。全てステートレス。
    def start_handshake(self):
        return {'me': 'suzukazeaoba'}

    def end_handshake(self, input_dict):
        return

    def setup(self, input_dict):
        return {
            'ready': input_dict['punter'],
            'state': input_dict.copy()}

    def move(self, input_dict):
        rivers = input_dict['state']['map']['rivers']
        moves = input_dict['move']['moves']
        # stateを更新
        for move in moves:
            if 'claim' not in move: continue
            claim = move['claim']
            for river in rivers:
                if (claim['source'] == river['source']
                    and claim['target'] == river['target']):
                    river['punter'] = claim['punter']
        # punterのsetされていないriverを選ぶ
        choices = [r for r in rivers if 'punter' not in r]
        if len(choices) == 0:
            return {'pass': {'punter': punter.id}}
        else:
            import random
            r = random.choice(choices)
            return {'claim': {
                'punter': input_dict['state']['punter'],
                'source': r['source'],
                'target': r['target']}}

class ProcessPunter(Punter):
    def __init__(self, command):
        self.command = command

    def start_handshake(self):
        raise "todo"

    def end_handshake(self, input_dict):
        raise "todo"

    def setup(self, input_dict):
        # 区別はAIバイナリ側でする
        return move(input_dict)

    def move(self, input_dict):
        # child_process.run(command, to_json(input_dict)) 的なこと
        raise "todo"

def emulate(punters, map_dict):
    # setup
    punter_id = 0
    for punter in punters:
        # handshake
        hand_req = punter.start_handshake()
        punter.name = hand_req['me']
        punter.id = punter_id
        punter_id += 1
        punter.last_move = {'pass': {'punter': punter.id}}
        punter.end_handshake({'you': punter.name})
        # setup
        ready = punter.setup({
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
        hand_req = punter.start_handshake()
        assert hand_req['me'] == punter.name
        punter.end_handshake({'you': punter.name})
        move = punter.move({
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
        print(move)
        all_moves.append(move)
        punter.last_move = move
        punter_index = (punter_index + 1) % len(punters)

    # score
    print(map_dict)


# python3 -M emulate map.json "./ai1" "./ai2"
# をargparseして↓のように呼び出す
emulate([Punter(), Punter()], {
    'sites': [{'id': 1}, {'id': 2}, {'id': 3}, {'id': 4}],
    'rivers': [
        {'source': 1, 'target': 2},
        {'source': 2, 'target': 3},
        {'source': 3, 'target': 4},
        {'source': 1, 'target': 4}],
    'mines': [1, 5]})
