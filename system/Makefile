include ../Makefile.in

system.a: signal.o handlers.o process.o
	ar ru $@ $^

signal.o: signal.c handlers.h ../common/error.h
handlers.o: handlers.c 
process.o: process.c

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
