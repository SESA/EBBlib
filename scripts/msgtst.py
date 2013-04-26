import pexpect
import multiprocessing
import sys

cores = multiprocessing.cpu_count()
sys.stdout.write( "msgtst on " + str(cores) + " cores [")
for i in range(cores):
    child = pexpect.spawn ('apps/msgtst/msgtst -ebbos_cores ' + str(i+1))
    for j in range(99):
        child.expect('msgtst ' + str(j%(i+1)) + ' -> ' + str((j+1)%(i+1)), timeout=10)
print "]"
