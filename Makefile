CC = gcc

# What build to peform, 
PROFILE=optimized
PROFILE=debug

DPDK_ROOT=/home/omid/nfv/dpdk/dpdk-18.02/x86_64-native-linuxapp-gcc

# TODO: Swap these with w/e flag DPDK spits out - should be a combination of mk/rte.vars.mk and ...
DPDK_CFLAGS=-m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I$(DPDK_ROOT)/include -include $(DPDK_ROOT)/include/rte_config.h -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wno-format-nonliteral -Wno-format-security -Wundef -Wwrite-strings -Wdeprecated

#-Wl,-rpath-link,$(DPDK_ROOT)/lib -Wl,-rpath,$(DPDK_ROOT)/lib 
DPDK_LDFLAGS=-L$(DPDK_ROOT)/lib -Wl,-lrte_flow_classify -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_gro -Wl,-lrte_gso -Wl,-lrte_meter -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,--whole-archive -Wl,-lrte_cfgfile -Wl,-lrte_hash -Wl,-lrte_member -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_bbdev -Wl,-lrte_cryptodev -Wl,-lrte_security -Wl,-lrte_eventdev -Wl,-lrte_rawdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_pci -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_sched -Wl,-lrte_kni -Wl,-lrte_bus_pci -Wl,-lrte_bus_vdev -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avf -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_failsafe -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_softnic -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_vdev_netvsc -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_bbdev_null -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,-lrte_mempool_octeontx -Wl,-lrte_pmd_octeontx -Wl,-lrte_pmd_opdl_event -Wl,-lrte_pmd_skeleton_rawdev -Wl,--no-whole-archive

# CFLAGS generation
CFLAGS_BASE = -I include/ -I lib/ -Itest-rxer/ -lm $(DPDK_CFLAGS) -fdiagnostics-color -flto -march=native
CFLAGS_OPT = $(CFLAGS_BASE) -O3 -ffunction-sections -fdata-sections -fPIC -ggdb -fno-omit-frame-pointer
CFLAGS_DEBUG = $(CFLAGS_BASE) -O0 -pg -ggdb -fPIC

# LDFLAGS generation
LDFLAGS+=$(DPDK_LDFLAGS) -lm -lrt -lnuma -lpcap -fPIC -flto

# Linker flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS += -Wl,--gc-sections
endif
ifeq ($(UNAME_S),Darwin)
	#LDFLAGS += -Wl,-dead_strip
endif

# Build the source tree
SRC  = $(wildcard src/*.c)
SRC += $(wildcard lib/*.c)

JIT_SRC = $(SRC)
JIT_SRC += $(wildcard src/elements/*.c)
JIT_SRC += jit_test.c

TXER_SRC = $(SRC)
TXER_SRC += $(wildcard src/elements/*.c)
TXER_SRC += txer.c

RXER_SRC = $(SRC)
RXER_SRC += $(wildcard src/elements/*.c)
RXER_SRC += rxer.c

RXER_TEST_SRC = $(SRC)
RXER_TEST_SRC += $(wildcard src/elements/*.c)
RXER_TEST_SRC += rxer_test.c
RXER_TEST_SRC += test-rxer/benchmark.c

MAIN_SRC = $(SRC)
MAIN_SRC += main.c

ifeq ($(MAKECMDGOALS), jit-test)
	JIT_SRC += tests/$(BENCHMARK)/benchmark.c
endif


# Object file rules
MAIN_OBJ = $(MAIN_SRC:%.c=$(BUILD_DIR)/%.o)
JIT_OBJ  = $(JIT_SRC:%.c=$(BUILD_DIR)/%.o)
TXER_OBJ = $(TXER_SRC:%.c=$(BUILD_DIR)/%.o)
RXER_OBJ = $(RXER_SRC:%.c=$(BUILD_DIR)/%.o)
RXER_TEST_OBJ = $(RXER_TEST_SRC:%.c=$(BUILD_DIR)/%.o)

ALLSRC=$(SRC)
ALLSRC+=$(wildcard *.c)
ALLSRC+= $(wildcard src/elements/*.c)
ALLSRC+=$(wildcard tests/*/benchmark.c)
ALLSRC+=$(wildcard test-rxer/benchmark.c)

ALLOBJ= $(ALLSRC:%.c=$(BUILD_DIR)/%.o)


BUILD_DIR = build
BIN_DIR = bin

# Choose the final CFLAGS based on the profile
ifeq ($(PROFILE), optimized)
	CFLAGS += $(CFLAGS_OPT)
else ifeq ($(PROFILE), debug)
	CFLAGS += $(CFLAGS_DEBUG)
else
	echo "Undefined profile selected.  Valid options are: optimized, debug."
	exit 1
endif

.PHONY: all
all: main

.PHONY: clean
clean:
	@find . -iname "*.o" -delete
	@rm -f main txer rxer jit.so

$(ALLOBJ): $(BUILD_DIR)/%.o: %.c
	@>&2 echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(EXTRA) $(CPPFLAGS) -c -o $@ $<


.PHONY: main
main: $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: jit-test
jit: $(JIT_OBJ)
	@$(CC) -shared -o $@.so $^ $(LDFLAGS) $(CFLAGS) $(EXTRA)

.PHONY: txer
txer: $(TXER_OBJ)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: rxer
rxer: $(RXER_OBJ)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: rxer-test
rxer-test: $(RXER_TEST_OBJ)
	@mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/$@ $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl
