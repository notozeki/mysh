include Makefile.in
MODULE_DIRS = analyzer io system common env embed
MODULE_FILES = $(foreach M,$(MODULE_DIRS),$(M)/$(M).a)
INCLUDES = common/string.h io/io.h analyzer/analyzer.h analyzer/node.h system/signal.h \
	   system/executer.h env/jobs.h env/flags.h
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
	-rm ezsh
	-rm *.o
	-rm *~