#!/bin/bash
MAX_POW2=6
NO_ITERATIONS=2
COUNT=0
while : 
do 
  for ((i=1; i<= $MAX_POW2; i++))
  do
    let "threads=2**$i"
    let ++COUNT
    echo "TEST:$COUNT TREADS:$threads ITER:$NO_ITERATIONS" 
    ./eosSSACTest $threads $NO_ITERATIONS
  done
done
