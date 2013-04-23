import pexpect
import multiprocessing
import sys

cores = multiprocessing.cpu_count()
sys.stdout.write( "helloworld on " + str(cores) + " cores [")
for i in range(cores):
    child = pexpect.spawn ('apps/helloworld/helloworld -ebbos_cores ' + str(i+1))
    child.expect('Hello world!', timeout=10)
print "]"
