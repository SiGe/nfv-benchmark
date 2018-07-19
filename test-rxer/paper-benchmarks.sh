#!/bin/bash

# Script directory
DIR="$( cd "$(dirname "$0")" ; pwd -P )"

# Batch size
BS=146
FLTO=0

mrc() {
    PREFIX=$1
    [ $PREFIX != "" ] && PREFIX="${PREFIX}_"
    echo "${PREFIX}measurement_large,${BS} ${PREFIX}routing_stanford,${BS} ${PREFIX}checksum,${BS}"
}

src() {
    PREFIX=$1
    [ $PREFIX != "" ] && PREFIX="${PREFIX}_"
    echo "${PREFIX}measurement_small,${BS} ${PREFIX}routing_stanford,${BS} ${PREFIX}checksum,${BS}"
}

# Pipeline with queue
pipeline-make-wq() {
    ${DIR}/make-pipeline.sh timer,${BS} $@ drop,${BS} 
}

# Pipeline without queue
pipeline-make-nq() {
    ${DIR}/make-pipeline-bypass-queue.sh timer,${BS} $@ drop,${BS} 
}

pipeline-run() {
    CMD_PREFIX=$@
    sudo ${CMD_PREFIX} ${DIR}/../bin/rxer-test -n4 -l1-3
}

CMD_PREFIX_PERF="perf record -e cycles:pp,cache-misses"
CMD_PREFIX_TIMEOUT="timeout -k20 20"

# Measure the throughput of the pipeline
pipeline-thr() {
    mean='thr'
    tmpFile=`mktemp`
    for i in `seq 1 5`; do
        pipeline-run "${CMD_PREFIX_TIMEOUT}" >${tmpFile} 2>&1
        val="`cat ${tmpFile} | grep Rate | grep In | awk '{print $14}' | tail -n10 | head -n6 | datamash -s trimmean 1`"
        mean="${mean}\n${val}"
    done
    paste <(echo -e "TempFile\n${tmpFile}\n") <(echo -e "${mean}" | datamash -H -s trimmean 1 mean 1 q1 1 median 1 q3 1 iqr 1 sstdev 1)
}

pipeline-stats() {
    header="$1"
    BS="$2"
    cmd="$3"
    is_mrc="$4"
    typ="$5"

    if [[ $is_mrc -eq 1 ]]; then
        pipeline="`mrc ${typ}`"
        name="MRC/$typ@${BS}"
    fi

    if [[ $is_mrc -eq 2 ]]; then
        pipeline="`src ${typ}`"
        name="SRC/$typ@${BS}"
    fi

    if [[ $is_mrc -eq 0 ]]; then
        pipeline="${typ},${BS}"
        name="$typ@${BS}"
    fi

    ${cmd} ${pipeline} >build.log 2>&1
    res="`pipeline-thr`"
    if [[ $header -eq 1 ]]; then
        paste <(echo -e "pipeline\tFLTO\n$name\t${FLTO}") <(echo -e "${res}") | column -t
    else
        paste <(echo -e "$name\t${FLTO}") <(echo -e "${res}" | tail -n 2) | column -t
    fi
}

set-flto() {
    if [[ $1 -eq 1 ]]; then
        sed -i '/FLTO=/c\FLTO=-flto' $DIR/../Makefile
        FLTO=1
    else
        sed -i '/FLTO=/c\FLTO=' $DIR/../Makefile
        FLTO=0
    fi
}

zipf-skewed-experiments() {
    set-flto 0
    pipeline-stats "1" "32"  "pipeline-make-nq" "1" "bpp"
    pipeline-stats "0" "32"  "pipeline-make-nq" "2" "bpp"
    # set-flto 1
    # pipeline-stats "0" "112" "pipeline-make-nq" "1" "bpp"
    # pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged"
    # pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged_opt"
    # pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged_fastpass"
}

zipf-uniform-experiments() {
    set-flto 0
    pipeline-stats "1" "1"   "pipeline-make-nq" "1" "naive"
    pipeline-stats "0" "32"  "pipeline-make-wq" "1" "batching"
    pipeline-stats "0" "32"  "pipeline-make-wq" "1" "bp"
    pipeline-stats "0" "32"  "pipeline-make-wq" "1" "bpp"
    pipeline-stats "0" "32"  "pipeline-make-nq" "1" "bpp"
    pipeline-stats "0" "112" "pipeline-make-nq" "1" "bpp"
    set-flto 1
    pipeline-stats "0" "112" "pipeline-make-nq" "1" "bpp"
    pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged"
    pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged_opt"
    pipeline-stats "0" "130" "pipeline-make-nq" "0" "merged_fastpass"
}

main() {
    zipf-skewed-experiments
}

main
