class PunterClient:
    def start_handshake(self):
        return {'me': 'suzukazeaoba'}

    def end_handshake(self, input_dict):
        return

    def setup(self, input_dict):
        return {
            'ready': input_dict['punter'],
            'state': input_dict.copy()}

    def play(self, input_dict):
        state = input_dict['state'].copy()
        rivers = state['map']['rivers']
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
            return {
                'claim': {
                    'punter': state['punter'],
                    'source': r['source'],
                    'target': r['target']
                },
                'state': state
            }
        
    def stop(self, input_dict):
        return
