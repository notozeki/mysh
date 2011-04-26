include ../Makefile.in

common.a: string.o error.o
	ar ru $@ $^

string.o: string.c string.h
error.o: error.c meta.h

clean:
	-rm *.o
	-rm *.a
	-rm *~
