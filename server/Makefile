#################################### USAGE #####################################

# Do not remove any variables from this section,
# If you dont want to use them, just leave them
# empty like the LDLIBS variable

# CC - used C compiler
# LDLIBS - libraries to be linked, for example -lm
# CFLAGS - flags used to compile program like -Wall
# LDFLAGS - these flags are added to the linker when creating the final binary
# 			Beware! These flags are only applied to the target, not to tests.
# 			This allows creation of code libraries with -shared for example
# 			and making independent test binary
# DEBUG_FLAGS - flags to be added if make is executed with DEBUG=y parameter

# Repository structure for this makefile to work correctly
# 1. All source files must be under SRCDIR directory
# 2. Files with source code for testing must be in TESTDIR directory

# You can visit this makefile repository and example project here
# https://github.com/CrusaderSVK287/Makefiles/tree/main/C_projects

################################### TARGETS ####################################

# all: compiles source code into object files and result binary, call just make
# test: compiles if necesarry, executes tests
# clean: cleans repository of temporary files, object files and binaries
# deps: lists dependencies needed for full makefile functionality

################################ CONFIGURATION #################################
# Compiler flags
CC = gcc
LDLIBS = -lcclog -lcjson -lcjson_utils
CFLAGS = -Wall -Werror -std=gnu17 
LDFLAGS =
DEBUG_FLAGS = -g3 -DDEBUG

# Source and build directories ending with /
# DO NOT ADD SPACE AFTER /, COULD RESULT IN UNWANTED DELETION OF FILES
SRCDIR  = src/
OBJDIR  = obj/
BINDIR  = bin/
TESTDIR = test/

# Result binary name
BINNAME = dhcps

################################################################################
#                                                                              #
#                  DO NOT CHANGE ANYTHING BELOW THIS LINE                      #
#                                                                              #
################################################################################

ifeq ($(DEBUG),y)
	CFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(__PIPELINE_BUILD), y)
	CFLAGS += -D__RUN_TIMER_TESTS__ -D__LEASES_TEST_BUILD -D__PIPELINE_BUILD
endif

ECHO_CMD:= @if [ -z "$(findstring s,$(filter-out --%,$(MAKEFLAGS)))" ]; then \
		   echo -e 
ECHO_CMD_TAIL:= ;fi

ifneq ($(INC_DIR),)
LDFLAGS+= -L$(INC_DIR) 
CFLAGS+= -I$(INC_DIR)
endif

### COMPILATION ###

SRCS  = $(shell find $(SRCDIR) -name '*.c')
OBJS  = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRCS))
TSRCS = $(shell find $(TESTDIR) -name '*.c')
TOBJS = $(patsubst $(TESTDIR)%.c, $(OBJDIR)$(TESTDIR)%.o, $(TSRCS)) \
	$(subst $(OBJDIR)main.o,,$(OBJS))

BIN = $(BINDIR)$(BINNAME)

all: -setup $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDLIBS) $(LDFLAGS) $(OBJS) $(TESTFLAGS) -o $@
	@echo -e "\n\nBuild successful, result generated in $(PWD)/$(BINDIR)\n\n\n"

$(OBJDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

### TESTS ###
$(OBJDIR)$(TESTDIR)%.o: $(TESTDIR)%.c
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -I $(SRCDIR) -c $< -o $@

test: -setup $(OBJS) $(TOBJS)
	$(CC) $(LDLIBS) $(LDFLAGS) -I $(SRCDIR) $(TOBJS) -o $(BINDIR)$@
	@echo "----- RUNNING TESTS -----"
	cp $(TESTDIR)/test_leases/persist1.lease.backup $(TESTDIR)/test_leases/persist1.lease
	cp $(TESTDIR)/test_leases/persist2.lease.backup $(TESTDIR)/test_leases/persist2.lease
	$(BINDIR)$@

### SET UP ###
-setup:
	mkdir -p $(SRCDIR) $(OBJDIR) $(BINDIR) $(TESTDIR)

### DEPENDENCIES ###
deps:
	@echo "Dependencies: make, $(CC)"
	@echo "Libraries needed: $(LDLIBS)"

install:
	mv $(BINDIR)/$(BINNAME) /usr/local/bin/$(BINNAME)
