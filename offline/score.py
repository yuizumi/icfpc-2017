def edges_equal(a, b):
    return ((a['source'] == b['source'] and a['target'] == b['target'])
        or (a['source'] == b['target'] and a['target'] == b['source']))

def floyd_warshall(graph):
    n = len(graph)
    for k in range(n):
        for i in range(n):
            for j in range(n):
                graph[i][j] = min(graph[i][j], graph[i][k] + graph[k][j])

class Score:
    def __init__(self, map_dict, punters):
        self.map = map_dict
        self.rivers = map_dict['rivers'].copy()
        self.punters = punters
        self.passes = [0] * punters
        self.valid_moves = []
        self.pre_calced = False
        self.settings = {'futures': False, 'splurges': True, 'options': True}

    def support(self, name):
        return self.settings[name]

    def move_all(self, moves):
        for move in moves: self.move(move)

    def claim(self, claim):
        rivers = [r for r in self.rivers
                  if edges_equal(r, claim) and 'punter' not in r]
        if len(rivers) == 1:
            rivers[0]['punter'] = claim['punter']
            return rivers[0]
        else:
            return None

    def option(self, option):
        rivers = [r for r in self.rivers
                  if edges_equal(r, option)
                  and 'punter' in r
                  and r['punter'] != option['punter']
                  and 'option' not in r]
        if len(rivers) == 1:
            rivers[0]['option'] = option['punter']
            return rivers[0]
        else:
            return None

    def splurge(self, splurge):
        route = splurge['route']
        claims = [
            {'source': x[0], 'target': x[1],
             'punter': splurge['punter']}
            for x in zip(route[:-1], route[1:])]
        if len(claims) < 1: return None
        if self.passes[splurge['punter']] < (len(claims) - 1):
            return None # error
        self.passes[splurge['punter']] -= (len(claims) - 1)
        rivers = []
        river = None
        for claim in claims:
            river = self.claim(claim) or self.option(claim)
            if river is None: break
            rivers.append(river)

        # sucsess
        if river is not None: return rivers

        # rollback
        for river in rivers:
            if 'option' in river:
                del river['option']
            else:
                del river['punter']
        return None # error

    def assert_punter(self, dic, punter_or_none):
        if punter_or_none is None: return
        assert dic['punter'] == punter_or_none.id

    # return corrected move
    def move(self, move, punter=None, logger=None, turn=None):
        if logger is not None: assert turn is not None

        if 'claim' in move:
            claim = move['claim']
            self.assert_punter(claim, punter)
            if self.claim(claim) is None:
                # error
                if logger is not None: logger.warn(turn, move)
                move = None

        elif 'pass' in move:
            self.assert_punter(move['pass'], punter)

        elif self.support('splurges') and 'splurge' in move:
            splurge = move['splurge']
            self.assert_punter(splurge, punter)
            if self.splurge(splurge) is None:
                # error
                if logger is not None: logger.warn(turn, move)
                move = None

        elif self.support('options') and 'option' in move:
            option = move['option']
            self.assert_punter(option, punter)
            if self.option(option) is None:
                # error
                if logger is not None: logger.warn(turn, move)
                move = None
        else:
            # error
            if logger is not None: logger.warn(turn, move)
            move = None

        # fill errored move
        if move is None and punter is not None:
            if logger is not None:
                logger.show('suspicious move',
                            {'turn': turn, 'name': punter.name})
            move = {'pass': {'punter': punter.id}, 'state': punter.state}

        # count pass
        if self.support('splurges'):
            assert move is not None
            if 'pass' in move:
                self.passes[move['pass']['punter']] += 1

        return move

    def fill_graph_with_river(self, graph, river):
        s = self.site_to_index[river['source']]
        t = self.site_to_index[river['target']]
        graph[s][t] = 1
        graph[t][s] = 1

    def pre_calc(self):
        if self.pre_calced: return
        self.pre_calced = True

        sites = self.map['sites']
        site_count = len(sites)
        self.site_to_index = {}
        for i in range(site_count):
            self.site_to_index[sites[i]['id']] = i

        graph = [[(0 if i == j else float('inf'))
                  for i in range(site_count)]
                 for j in range(site_count)]
        for river in self.rivers:
            self.fill_graph_with_river(graph, river)
        floyd_warshall(graph)
        self.dist = graph

    def calc(self, punter):
        self.pre_calc()
        site_count = len(self.map['sites'])
        subgraph = [[(0 if i == j else float('inf'))
                     for i in range(site_count)]
                    for j in range(site_count)]
        for river in self.rivers:
            if river.get('punter') == punter or river.get('option') == punter:
                self.fill_graph_with_river(subgraph, river)

        floyd_warshall(subgraph)
        score = 0
        for mine in self.map['mines']:
            mine_index = self.site_to_index[mine]
            for i in range(len(self.map['sites'])):
                if subgraph[mine_index][i] != float('inf'):
                    score += int(self.dist[mine_index][i]) ** 2
        return score
