import log_to_dir
import json
import sys
from score import Score

map_dict = None
moves = []
last_claim = None # this should be added to moves
logger = log_to_dir.default_logger()
logger.start()

# stdin should be lamduct stderr
for line in sys.stdin:
    if 'Game server:' not in line and 'Client:' not in line:
        continue
    start = line.find('{')
    if start < 0: continue
    json_str = line[start: line.rfind('}') + 1]
    json_dict = json.loads(json_str)
        continue
    if map_dict is None and 'map' in json_dict:
        map_dict = json_dict['map']
        logger.info('map', map_dict)
    if 'move' in json_dict:
        assert map_dict is not None
        for move in json_dict['move']['moves']:
            logger.turn(len(moves), move)
            moves.append(move)
    if 'stop' in json_dict:
        assert map_dict is not None
        for move in json_dict['stop']['moves']:
            logger.turn(len(moves), move)
            moves.append(move)
        logger.info('scores', json_dict['stop']['scores'])

logger.turn(len(moves), last_claim)
moves.append(last_claim)
logger.show('rivers', len(map_dict['rivers']))
logger.show('turn', len(moves))
logger.end()
