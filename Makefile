include Makefile.in
INCLUDES=analyzer/lex.h analyzer/token.h common/string.h io/io.h
ARCHIVES=analyzer/analyzer.a common/common.a io/io.a

mysh: mysh.c $(ARCHIVES) $(INCLUDES)
	$(CC) $(DEBUG) $(CFLAGS) -o $@ mysh.c $(ARCHIVES)

analyzer/analyzer.a:
	cd analyzer; make

common/common.a:
	cd common; make

io/io.a:
	cd io; make

clean:
	cd analyzer; make clean
	cd common; make clean
	cd io; make clean
	-rm mysh
	-rm *.o
	-rm *~