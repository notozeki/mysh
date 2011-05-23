include ../Makefile.in

system.a: signal.o handlers.o process.o redv.o executer.o
	ar ru $@ $^

signal.o: signal.c handlers.h ../common/error.h
handlers.o: handlers.c 
process.o: process.c
redv.o: redv.c redv.h ../common/error.h
executer.o: executer.c redv.h ../analyzer/node.h ../common/error.h 

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
