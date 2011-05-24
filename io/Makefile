include ../Makefile.in

io.a: io.o
	ar ru $@ $^

io.o: io.c io.h ../common/string.h

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
