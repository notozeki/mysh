include ../Makefile.in

embed.a: embed.o actions.o
	ar ru $@ $^

embed.o: embed.c actions.h
actions.o: actions.c ../env/flags.h

.PHONY: clean
clean:
	-rm *.o
	-rm *.a
	-rm *~
