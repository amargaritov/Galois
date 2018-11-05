#!/bin/sh -e

graph=$1
PREFIX="numactl --membind=0 --physcpubind=0"

$PREFIX ./build/lonestar/bfs/bfs -t 1 $graph -noverify
