include Makefile.in
MODULE_DIRS = analyzer io system common
MODULE_FILES = $(foreach M,$(MODULE_DIRS),$(M)/$(M).a)
INCLUDES = analyzer/lex.h analyzer/token.h common/string.h io/io.h system/signal.h
PROGRAM = mysh

all: $(notdir $(MODULE_FILES)) $(PROGRAM)

$(PROGRAM): mysh.c $(MODULE_FILES) $(INCLUDES)
	$(CC) $(DEBUG) $(CFLAGS) -o $@ mysh.c $(MODULE_FILES)

$(notdir $(MODULE_FILES)):
	$(MAKE) -C $(basename $@)

.PHONY: clean
clean:
	@for subdir in $(MODULE_DIRS) ; do \
		(cd $$subdir && $(MAKE) clean); \
	done
	-rm mysh
	-rm *.o
	-rm *~