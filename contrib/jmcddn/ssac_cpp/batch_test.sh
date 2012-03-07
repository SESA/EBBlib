#!/bin/bash
MAX_POW2=6
MAX_COUNT=6
NO_ITERATIONS=1
COUNT=0

while : 
do 
  for ((i=1; i<= $MAX_POW2; i++))
  do
    let threads=2**$i
    let binding=$i%2;
    let ++COUNT
    res1=$(date +%s.%N)
    ./eosSSACTest $threads $NO_ITERATIONS $binding >> test_output.csv
    res2=$(date +%s.%N)
    echo "$COUNT TREADS:$threads ITER:$NO_ITERATIONS BIND:$binding TIME: $(echo "$res2 - $res1"|bc )"
  done
done
