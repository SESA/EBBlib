classes = SSACSimpleSharedArray.C CacheSimple.C Test.C
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.C,%.o,$(src))
libs = -lpthread 
CXXFLAGS= -g

eosSSACTest: $(objs) $(hdrs)
	g++ ${CXXFLAGS} $(objs) $(libs) -o eosSSACTest 

clean:
	rm *.o eosSSACTest

