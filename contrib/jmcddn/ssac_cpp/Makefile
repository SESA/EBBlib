classes = SSACSimpleSharedArray.C CacheSimple.C Test.C 
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.C,%.o,$(src))
libs = -lpthread 
CXXFLAGS=-g -O0 -Wall

all: eosSSACTest

-include $(objs:.o=.d)

# compile and generate dependency info
%.o: %.C
	g++ -c $(CXXFLAGS) $*.c -o $*.o
	g++ -MM $(CXXFLAGS) $*.c > $*.d

eosSSACTest.o: Test.H 

eosSSACTest: $(objs) $(hdrs)
	g++ ${CXXFLAGS} $(objs) $(libs) -o eosSSACTest 

clean:
	rm -f *.o *.d eosSSACTest




