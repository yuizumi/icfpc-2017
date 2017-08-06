import json
import sys
from score import Score

map_dict = None
answers = None
score = None

# stdin should be lamdact stderr
for line in sys.stdin:
    try:
        json_str = line[line.find('{'): line.rfind('}') + 1]
        json_dict = json.loads(json_str)
    except:
        continue
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

result = []
for answer in answers:
    punter = answer['punter']
    calced = score.calc(punter)
    result.append({'punter': punter, 'score': calced})
    print('calced:', calced, 'answer:', answer['score'])

assert result == answer
