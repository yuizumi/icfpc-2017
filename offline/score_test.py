import json
import sys
from score import Score

map_dict = None
moves = []
last_claim = None # this should be added to moves
answers = None
score = None

# stdin should be lamduct stderr
for line in sys.stdin:
    if 'Game server:' not in line and 'Client:' not in line:
        continue
    start = line.find('{')
    if start < 0: continue
    json_str = line[start: line.rfind('}') + 1]
    json_dict = json.loads(json_str)
    if map_dict is None and 'map' in json_dict:
        map_dict = json_dict['map']
        score = Score(map_dict, json_dict['punters'])
    if 'move' in json_dict:
        assert map_dict is not None
        score.move_all(json_dict['move']['moves'])
    if 'stop' in json_dict:
        assert map_dict is not None
        score.move_all(json_dict['stop']['moves'])
        if answers is None:
            answers = json_dict['stop']['scores']
    if 'claim' in json_dict:
        last_claim = json_dict

score.move(last_claim)

result_list = []
for answer in answers:
    punter = answer['punter']
    calced = score.calc(punter)
    result_list.append(calced)
    print('calced:', calced, 'answer:', answer['score'])

assert result_list == [answer['score'] for answer in answers]
