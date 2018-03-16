#!/bin/bash

CWD=$(pwd)
BENCHMARK=$(echo ${CWD} | rev | cut -d'/' -f1 | rev)

PACKET_SIZE=(64 256 512 1400)
SWEEP_BUFFERS=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096)

OUTDIR=${CWD}/output
mkdir -p ${OUTDIR}

cd ${CWD}/../../

for pkt in ${PACKET_SIZE[@]}; do
    printf "CHECKSUM\tROUTING\tPACKET_SIZE\tCYCLES\n"
    for chksum in ${SWEEP_BUFFERS[@]}; do
        for routing in ${SWEEP_BUFFERS[@]}; do
            printf "$chksum\t$routing\t$pkt\t"
            make profile-run BENCHMARK=${BENCHMARK} EXTRA="-DREPEAT=100 -DPACKET_SIZE=${pkt} -DROUTING_BUFFER_SIZE=${routing} -DCHECKSUM_BUFFER_SIZE=${chksum}" | grep cycles | rev | cut -d' ' -f1 | sed -e 's/[()]//g' | rev
        done
    done | tee "${OUTDIR}/$pkt.tsv"
done

cd ${CWD}
