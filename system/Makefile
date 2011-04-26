include ../Makefile.in

system.a: signal.o handlers.o
	ar ru $@ $^

signal.o: signal.c handlers.h ../common/error.h
handlers.o: handlers.c 

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
