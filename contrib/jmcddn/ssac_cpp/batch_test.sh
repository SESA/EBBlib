#!/bin/bash
MAXTHREADS=1024
NO_ITERATIONS=2
while : 
do 
  for ((i=1; i<= $MAXTHREADS; i++))
  do
   #  echo $i $NO_ITERATIONS 
    ./eosSSACTest $i $NO_ITERATIONS 
  done
done
