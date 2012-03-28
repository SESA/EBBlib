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
    ./eosSSACTest $threads $NO_ITERATIONS 0 >> test_output.csv
    res2=$(date +%s.%N)
    res12=$(echo "$res2-$res1"|bc)
    res3=$(date +%s.%N)
    ./eosSSACTest $threads $NO_ITERATIONS 1 >> test_output.csv
    res4=$(date +%s.%N)
    res34=$(echo "$res4-$res3"|bc)
    resdif=$(echo "$res12-$res34"|bc)
    echo "$COUNT TREADS:$threads ITER:$NO_ITERATIONS BOUND: $(echo "$res34"|bc) UNBOUND: $(echo "$res12"|bc) DIFF: $(echo "$resdif"|bc )" 
  done
done
