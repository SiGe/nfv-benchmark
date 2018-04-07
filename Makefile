CC = gcc

# What build to peform, 
PROFILE=optimized

# CFlag rules
CFLAGS_BASE = -I include/ -I lib/
CFLAGS_OPT = $(CFLAGS_BASE) -O3 -ffunction-sections -fdata-sections -fPIC
CFLAGS_DEBUG = $(CFLAGS_BASE) -O0 -pg -g -fPIC

# Linker flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS += -Wl,--gc-sections
endif
ifeq ($(UNAME_S),Darwin)
	#LDFLAGS += -Wl,-dead_strip
endif

TARGET = main

# Build the source tree
SRC  = $(wildcard src/*.c)
SRC += $(wildcard lib/*.c)

JIT_TEST_SRC = $(SRC)
JIT_TEST_SRC += $(wildcard src/elements/*.c)
JIT_TEST_SRC += jit_test.c

MAIN_SRC = $(SRC)
MAIN_SRC += main.c

ifeq ($(MAKECMDGOALS), jit-test)
	TARGET = jit-test
	JIT_TEST_SRC += tests/$(BENCHMARK)/benchmark.c
endif

# Object file rules
MAIN_OBJ = $(MAIN_SRC:.c=.o)
JIT_TEST_OBJ = $(JIT_TEST_SRC:.c=.o)

# List of perf events
EVENTS=cycles
EVENTS+=instructions
EVENTS+=cache-misses
EVENTS+=cache-references
EVENTS+=page-faults
EVENTS+=context-switches
EVENTS+=branches
EVENTS+=branch-misses
EVENTS+=cache-references
EVENTS+=L1-dcache-load-misses
EVENTS+=L1-dcache-loads
EVENTS+=L1-dcache-stores
EVENTS+=L1-icache-load-misses

comma:= ,
empty:=
space:= $(empty) $(empty)
PERF_EVENTS= $(subst $(space),$(comma),$(EVENTS))

# Choose the final CFLAGS based on the profile
ifeq ($(PROFILE), optimized)
	CFLAGS = $(CFLAGS_OPT)
else ifeq ($(PROFILE), debug)
	CFLAGS = $(CFLAGS_DEBUG)
else
	echo "Undefined profile selected.  Valid options are: optimized, debug."
	exit 1
endif

.PHONY: all
all: profile

no-profile: clean build
	./$(TARGET)

.PHONY: clean
clean:
	#@rm -f $(OBJ) $(TARGET)
	@rm -f $(MAIN_OBJ)
	@rm -f $(JIT_TEST_OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA) $(CPPFLAGS) -c -o $@ $<

.PHONY: build
build: $(MAIN_OBJ)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: profile
profile: clean build
	@echo flags,$(PROFILE),$(EXTRA)$
	perf stat -e$(PERF_EVENTS) ./$(TARGET)

.PHONY: profile-run
profile-run: clean build
	@echo flags,$(PROFILE),$(EXTRA)$
	./$(TARGET)

.PHONY: jit-test-build
jit-test-build: $(JIT_TEST_OBJ)
	$(CC) -shared -o $(TARGET).so $^ $(LDFLAGS) $(CFLAGS) $(EXTRA)

.PHONY: jit-test
jit-test: jit-test-build
