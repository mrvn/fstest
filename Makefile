# fstest makefile

# Binaries, sources and objects
fstest_SRCS := options.c list.c vector.c stats.c file.c dir.c fstest.c
fstest_OBJS := $(patsubst %.c,%.o,$(fstest_SRCS))

# Set VERBOSE if you want to see the commands being executed
ifdef VERBOSE
  L = @:
  Q =
else
  L = @echo
  Q = @
  MAKEFLAGS += --no-print-directory
endif
export L Q

# Default flags
DEPENDFLAGS := -MD -MP
BASEFLAGS := -O2 -g -pipe -D_LARGEFILE64_SOURCE=1 -D__USE_FILE_OFFSET64=1
WARNFLAGS := -W -Wall -Werror
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(DEPENDFLAGS)

# Toplevel rule to call by default
all: fstest

%.o: %.c
	$(L) Compiling $@
	$(Q) $(CC) $(CFLAGS) -MT $@ -MF $@.d -c $< -o $@

fstest: $(fstest_OBJS)
	$(L) Linking fstest
	$(Q) gcc -g -o $@ $+

clean:
	rm -rf fstest $(fstest_OBJS)

real-clean: clean
	rm -rf *.d *~

# Include depends
include $(wildcard *.d)
