classes = SSACSimpleSharedArray.C CacheSimple.C
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.C,%.o,$(src))
CXXFLAGS=-O4

eosSSACTest: $(objs) $(hdrs)
	g++ ${CXXFLAGS} $(objs) -o eosSSACTest 

clean:
	rm *.o eosSSACTest

