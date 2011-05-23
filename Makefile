include Makefile.in
MODULE_DIRS = analyzer io system common
MODULE_FILES = $(foreach M,$(MODULE_DIRS),$(M)/$(M).a)
INCLUDES = analyzer/analyzer.h common/string.h io/io.h system/signal.h
PROGRAM = ezsh

all: $(notdir $(MODULE_FILES)) $(PROGRAM)

$(PROGRAM): main.c $(MODULE_FILES) $(INCLUDES)
	$(CC) $(DEBUG) $(CFLAGS) -o $@ main.c $(MODULE_FILES)

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