CC = gcc

# What build to peform, 
PROFILE=optimized

# CFlag rules
CFLAGS_BASE = -I include/ -I lib/
CFLAGS_OPT = $(CFLAGS_BASE) -O3
CFLAGS_DEBUG = $(CFLAGS_BASE) -O0 -pg -g

TARGET = main

# Build the source tree
SRC  = $(wildcard src/*.c)
SRC += $(wildcard lib/*.c)
SRC += $(wildcard src/elements/*.c)
SRC += main.c

ifeq ($(MAKECMDGOALS), profile-run)
	SRC += tests/$(BENCHMARK)/benchmark.c
endif

# Object file rules
OBJ  = $(SRC:.c=.o)

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
	@rm -f $(OBJ) $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA) $(CPPFLAGS) -c -o $@ $<

.PHONY: build
build: $(OBJ)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(CFLAGS) $(EXTRA)

.PHONY: profile
profile: clean build
	@echo flags,$(PROFILE),$(EXTRA)$
	perf stat -e$(PERF_EVENTS) ./$(TARGET)

.PHONY: profile-run
profile-run: clean build
	@echo flags,$(PROFILE),$(EXTRA)$
	./$(TARGET)
