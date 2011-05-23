include ../Makefile.in

analyzer.a: analyzer.o token.o node.o lexer.o parser.o
	ar ru $@ $^

analyzer.o: analyzer.c node.h lexer.h parser.h
token.o: token.c token.h
node.o: node.c node.h ../common/error.h
lexer.o: lexer.c token.h ../common/string.h ../common/error.h
parser.o: parser.c lexer.h token.h node.h

clean:
	-rm *.o
	-rm *.a
	-rm *~
