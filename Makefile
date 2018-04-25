CC = gcc

# What build to peform, 
PROFILE=optimized
PROFILE=debug

DPDK_ROOT=/home/omid/nfv/dpdk/dpdk-18.02/x86_64-native-linuxapp-gcc

# TODO: Swap these with w/e flag DPDK spits out - should be a combination of mk/rte.vars.mk and ...
DPDK_CFLAGS=-m64 -pthread  -march=native -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -DRTE_MACHINE_CPUFLAG_AES -DRTE_MACHINE_CPUFLAG_PCLMULQDQ -DRTE_MACHINE_CPUFLAG_AVX -DRTE_MACHINE_CPUFLAG_RDRAND -DRTE_MACHINE_CPUFLAG_FSGSBASE -DRTE_MACHINE_CPUFLAG_F16C -DRTE_MACHINE_CPUFLAG_AVX2  -I$(DPDK_ROOT)/include -include $(DPDK_ROOT)/include/rte_config.h -W -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wpointer-arith -Wcast-align -Wnested-externs -Wcast-qual -Wno-format-nonliteral -Wno-format-security -Wundef -Wwrite-strings -Wdeprecated

DPDK_LDFLAGS=-L$(DPDK_ROOT)/lib -Wl,-lrte_flow_classify -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_gro -Wl,-lrte_gso -Wl,-lrte_meter -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,--whole-archive -Wl,-lrte_cfgfile -Wl,-lrte_hash -Wl,-lrte_member -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_bbdev -Wl,-lrte_cryptodev -Wl,-lrte_security -Wl,-lrte_eventdev -Wl,-lrte_rawdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_pci -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_sched -Wl,-lrte_kni -Wl,-lrte_bus_pci -Wl,-lrte_bus_vdev -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avf -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_failsafe -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_softnic -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_vdev_netvsc -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_bbdev_null -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,-lrte_mempool_octeontx -Wl,-lrte_pmd_octeontx -Wl,-lrte_pmd_opdl_event -Wl,-lrte_pmd_skeleton_rawdev -Wl,--no-whole-archive

# CFLAGS generation
CFLAGS_BASE = -I include/ -I lib/ -lm $(DPDK_CFLAGS) -fdiagnostics-color
CFLAGS_OPT = $(CFLAGS_BASE) -O3 -ffunction-sections -fdata-sections -fPIC
CFLAGS_DEBUG = $(CFLAGS_BASE) -O0 -pg -g -fPIC

# LDFLAGS generation
LDFLAGS+=$(DPDK_LDFLAGS) -lm -lrt -lnuma -lpcap -fPIC

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

TXER_SRC = $(SRC)
TXER_SRC += $(wildcard src/elements/*.c)
TXER_SRC += txer.c

MAIN_SRC = $(SRC)
MAIN_SRC += main.c

ifeq ($(MAKECMDGOALS), jit-test)
	TARGET = jit-test
	JIT_TEST_SRC += tests/$(BENCHMARK)/benchmark.c
endif

ifeq ($(MAKECMDGOALS), txer)
	TARGET = txer
endif

# Object file rules
MAIN_OBJ = $(MAIN_SRC:.c=.o)
JIT_TEST_OBJ = $(JIT_TEST_SRC:.c=.o)
TXER_OBJ = $(TXER_SRC:.c=.o)

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
all: build

no-profile: clean build
	./$(TARGET)

.PHONY: clean
clean:
	@find . -iname "*.o" -delete

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA) $(CPPFLAGS) -c -o $@ $<

.PHONY: build
build: $(MAIN_OBJ)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: jit-test-build
jit-test-build: $(JIT_TEST_OBJ)
	$(CC) -shared -o $(TARGET).so $^ $(LDFLAGS) $(CFLAGS) $(EXTRA)

.PHONY: jit-test
jit-test: jit-test-build

.PHONY: txer-build
txer-build: $(TXER_OBJ)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(CFLAGS) $(EXTRA) -ldl

.PHONY: txer
txer: txer-build
