include ../Makefile.in

analyzer.a: lex.o token.o
	ar ru $@ lex.o token.o

lex.o: lex.c token.h ../common/string.h
	$(CC) $(DEBUG) $(CFLAGS) -c -o $@ lex.c

token.o: token.c token.h
	$(CC) $(DEBUG) $(CFLAGS) -c -o $@ token.c

clean:
	-rm *.o
	-rm *.a
	-rm *~
