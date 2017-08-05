# build

```
$ cd ai
$ cmake .
$ make
```

# execute

Please install [lamduct](https://github.com/icfpcontest2017/lambda-duct) before.

```
$ lamduct ./some_ai --log-level 3 --game-port 9XXX
```

You can get port numbers from http://punter.inf.ed.ac.uk/status.html.

# offline

```
$ wget http://punter.inf.ed.ac.uk/maps/lambda.json
$ python3 offline/emulate.py lambda.json ai/greedy ai/forest
```
