#!/bin/bash

CWD=$(pwd)
BENCHMARK=$(echo ${CWD} | rev | cut -d'/' -f1 | rev)

PACKET_SIZE=(64 256 512 1400)
SWEEP_BUFFERS=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096)

OUTDIR=${CWD}/output
mkdir -p ${OUTDIR}

cd ${CWD}/../..

PIPELINE=$(echo $CWD | rev | cut -d'/' -f1 | rev)
MOD1=$(echo $PIPELINE | cut -d'-' -f1)
MOD2=$(echo $PIPELINE | cut -d'-' -f2)

for pkt in ${PACKET_SIZE[@]}; do
    printf "$MOD1\t$MOD2\tPACKET_SIZE\tCYCLES\n"
    for mod1 in ${SWEEP_BUFFERS[@]}; do
        for mod2 in ${SWEEP_BUFFERS[@]}; do
            printf "$mod1\t$mod2\t$pkt\t"
            make profile-run BENCHMARK=${BENCHMARK} EXTRA="-DREPEAT=200 -DPACKET_SIZE=${pkt} -DMOD_BUFFER_SIZE_2=${mod2} -DMOD_BUFFER_SIZE_1=${mod1}" | grep cycles | rev | cut -d' ' -f1 | sed -e 's/[()]//g' | rev
        done
    done | tee "${OUTDIR}/$pkt.tsv"
done

cd ${CWD}
