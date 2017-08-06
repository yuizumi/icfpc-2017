#!/bin/bash

get() {
    wget -c -N -P $(dirname $0)/../maps $1
}

get_official() {
    get http://punter.inf.ed.ac.uk/maps/$1
}

get_official sample.json
get_official lambda.json
get_official Sierpinski-triangle.json
get_official circle.json
get_official randomMedium.json
get_official randomSparse.json
get_official boston-sparse.json
get_official tube.json
get_official edinburgh-sparse.json
get_official nara-sparse.json
get_official oxford-sparse.json
get_official gothenburg-sparse.json
