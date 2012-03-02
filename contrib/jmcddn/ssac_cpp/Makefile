classes = SSACSimpleSharedArray.C CacheSimple.C Test.C 
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.C,%.o,$(src))
libs = -lpthread 
CXXFLAGS=-g -O0 -Wall

all: eosSSACTest

eosSSACTest.o: Test.H

eosSSACTest: $(objs) $(hdrs)
	g++ ${CXXFLAGS} $(objs) $(libs) -o eosSSACTest 

clean:
	rm -f *.o eosSSACTest

