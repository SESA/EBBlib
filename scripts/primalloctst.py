import os
import pexpect
import multiprocessing
import sys

alloc = 0
free = 0
cores = multiprocessing.cpu_count()
sys.stdout.write( "primalloctst on " + str(cores) + " cores [")
for i in range(cores):
    child = pexpect.spawn ('apps/primalloctst/primalloctst -ebbos_cores ' + str(i+1))
    # for now, just expect this number of allocates
    for j in range(1270):
        i = child.expect(['allocated pointer', 'freed pointer'], timeout=10)
        if i==0:
            alloc +=1
        else:
            free +=1

print "alloc is "+str(alloc) + " free is "+ str(free)
if alloc != free:
        os._exit(-1)
    
