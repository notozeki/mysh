include ../Makefile.in

io.a: io.o
	ar ru $@ io.o

io.o: io.c io.h ../common/string.h
	$(CC) $(DEBUG) $(CFLAGS) -c -o $@ io.c

clean:
	-rm *.o
	-rm *.a
	-rm *~
