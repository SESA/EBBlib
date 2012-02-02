classes = SSACSimpleSharedArray.C
hdrs = $(patsubst %.c,%.h,$(classes))
src = eosSSACTest.C $(classes)
objs = $(patsubst %.c,%.o,$(src))

eosSSACTest: $(src) $(hdrs)
	g++ $(objs) -o eosSSACTest 




