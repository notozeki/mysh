include ../Makefile.in

env.a: jobs.o flags.o
	ar ru $@ $^

jobs.o: jobs.c
flags.o: flags.c

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
