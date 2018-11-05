#!/bin/sh -e

graph=$1
PREFIX=numactl --membind=0 --physcpubind=0

$PREFIX ./build/lonestar/pagerank/pagerank-push -t 1 $graph -tolerance=0.001
