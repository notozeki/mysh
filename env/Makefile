include ../Makefile.in

env.a: jobs.o flags.o
	ar ru $@ $^

jobs.o: jobs.c ../common/error.h
flags.o: flags.c flags.h

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
