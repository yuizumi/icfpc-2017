import queue

def edges_equal(a, b):
    return ((a['source'] == b['source'] and a['target'] == b['target'])
        or (a['source'] == b['target'] and a['target'] == b['source']))

def calc_distance(graph, mines):
    dist_rows = {}
    for mine in mines:
        dist_row = {mine: 0}
        q = queue.Queue()
        q.put((mine, 0))
        while not q.empty():
            site, dist = q.get()
            for adj in graph[site]:
                if adj in dist_row:
                    continue
                dist_row[adj] = dist + 1
                q.put((adj, dist + 1))
        dist_rows[mine] = dist_row
    return dist_rows

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

    def generate_graph(self, rivers):
        site_count = len(self.map['sites'])
        graph = [[] for _ in range(site_count)]
        for river in rivers:
            s = self.site_to_index[river['source']]
            t = self.site_to_index[river['target']]
            graph[s].append(t); graph[t].append(s)
        return graph

    def pre_calc(self):
        if self.pre_calced: return
        self.pre_calced = True

        sites = self.map['sites']
        site_count = len(sites)
        self.site_to_index = {}
        for i in range(site_count):
            self.site_to_index[sites[i]['id']] = i

        graph = self.generate_graph(self.rivers)
        mines = (self.site_to_index[mine] for mine in self.map['mines'])
        self.dist = calc_distance(graph, mines)

    def calc(self, punter):
        self.pre_calc()
        site_count = len(self.map['sites'])
        subgraph = self.generate_graph(
            river for river in self.rivers
            if river.get('punter') == punter or river.get('option') == punter)
        mines = (self.site_to_index[mine] for mine in self.map['mines'])
        subdist = calc_distance(subgraph, mines)
        score = 0
        for mine in self.map['mines']:
            mine_index = self.site_to_index[mine]
            for i in range(len(self.map['sites'])):
                if subdist[mine_index].get(i) is not None:
                    score += int(self.dist[mine_index][i]) ** 2
        return score
