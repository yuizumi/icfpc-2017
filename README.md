[![Build Status](http://52.199.48.163/api/badges/yuizumi/icfpc-2017/status.svg)](http://52.199.48.163/yuizumi/icfpc-2017)

# members

* @alotofwe
* @bakaming
* @cocodrips
* @eomole
* @n__yoda
* @yuizumi_y5i

# build

Note the `cmake`'s argument is a parent directory (*two* dots).

```
$ cd build
$ cmake ..
$ make
```

# execute

Please install [lamduct](https://github.com/icfpcontest2017/lambda-duct) before.

```
$ lamduct ./some_ai --log-level 3 --game-port 9XXX
$
$ # if you want to use visualizer:
$ lamduct ./some_ai --log-level 3 --game-port 9XXX 2>&1 | \
  python3 offline/visualize_lamduct.py
```

You can get port numbers from http://punter.inf.ed.ac.uk/status.html.

# offline

```sh
$ tools/wget_maps.sh
$
$ # Emulate with logging into visualizer/log.
$ python3 offline/emulate.py maps/lambda.json build/ai/greedy build/ai/forest
$
$ # Emulate only using stderr (debug info) and stdout (scores).
$ python3 offline/emulate_no_write.py maps/lambda.json build/ai/greedy build/ai/forest
```
