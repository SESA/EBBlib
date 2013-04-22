import pexpect
import multiprocessing
import sys

cores = multiprocessing.cpu_count()
sys.stdout.write( "bindtst on " + str(cores) + " cores [")
for i in range(cores):
    child = pexpect.spawn ('apps/bindtst/bindtst -ebbos_cores ' + str(i+1))
    for j in range(i+1):
        child.expect('bindtst: PASSED', timeout=10)
print "]"
