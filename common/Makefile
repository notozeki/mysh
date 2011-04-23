include ../Makefile.in

common.a: string.o
	ar ru $@ string.o

string.o: string.c string.h
	$(CC) $(DEBUG) $(CFLAGS) -c -o $@ string.c

clean:
	-rm *.o
	-rm *.a
	-rm *~
