#----------------------------------------
# Benchmark
#----------------------------------------
NPARAMS=$#

usage() {
    cat <<EOF
Usage: $0 [module-1,batch-size-1] [module-2,batch-size-2] [...]

A few notes:
    - The batch size is ignored for naive_* modules
EOF
}

_benchmark_begin() {
    # Start of the benchmark file
    cat - <<EOF
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rte_ethdev.h"
#include "rte_malloc.h"

#include "benchmark.h"
#include "benchmark_helper.h"
#include "dataplane.h"
#include "pipeline.h"

struct pipeline_t *pipeline_build(struct rx_packet_stream *);

struct pipeline_t *pipeline_build(struct rx_packet_stream *stream) {
    struct pipeline_t *pipe = pipeline_create();
EOF
}


_benchmark_end() {
    cat - <<EOF
    return pipe;
}
EOF
}

benchmark_create() {
    _benchmark_begin

    num=0
    while [ $num -lt $NPARAMS ]; do
        (( num++ ))
        func=$(echo ${!num} | cut -d',' -f1)
        declare -f ${func} > /dev/null
        if [ $? -ne 0 ]; then
            >&2 echo "Module does not exist: ${func}"
            exit -1
        fi
        ${func} $num
    done

    _benchmark_end
}

benchmark_compile() {
    cd ${DIR}/..

    define_args=""
    num=0
    while [ $num -lt $NPARAMS ]; do
        (( num++ ))
        count=$(echo ${!num} | cut -d',' -f2)
        define_args="${define_args} -DMOD_BUFFER_SIZE_${num}=${count}"
    done

    echo ${define_args}
    make clean
    make rxer-test PROFILE=optimized EXTRA="${define_args}"
    #make rxer-test PROFILE=debug EXTRA="${define_args}"
}
