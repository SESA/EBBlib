classes = SSACSimpleSharedArray.C CacheSimple.C Test.C
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.C,%.o,$(src))
CXXFLAGS= -g

eosSSACTest: $(objs) $(hdrs)
	g++ ${CXXFLAGS} $(objs) -o eosSSACTest 

clean:
	rm *.o eosSSACTest

