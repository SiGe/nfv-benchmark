#!/bin/bash

BENCHMARK=$1
DIR="$( cd "$(dirname "$0")" ; pwd -P )"

export ELEMENT_PREFIX=""
source "$DIR/benchmark-incl/elements.sh"
source "$DIR/benchmark-incl/common.sh"

if [ $NPARAMS -lt "1" ]; then
    usage
    exit 1
fi

benchmark_create $@ > ${DIR}/benchmark.c
benchmark_compile $@
