#!/bin/sh -e

graph=$1
PREFIX="numactl --membind=0 --physcpubind=0"

$PREFIX ./build/lonestar/pagerank/pagerank-pull --t 1 $graph -tolerance=0.001 -noverify -algo=Topo
