obj = test.o bp_pub.o mempool.o

test : $(obj)
	cc -o test -lpthread $(obj)

test.o : bp_pub.h mempool.h
bp_pub.o : bp_pub.h
mempool.o : mempool.h bp_pub.h

.PHONY : clean
clean :
	rm test $(obj)

