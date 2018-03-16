#!/bin/bash

CWD=$(pwd)
BENCHMARK=$(echo ${CWD} | rev | cut -d'/' -f1 | rev)

PACKET_SIZE=(64 256 512 1400)
SWEEP_BUFFERS=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096)

OUTDIR=${CWD}/output
mkdir -p ${OUTDIR}

cd ${CWD}/../../
rm main

for pkt in ${PACKET_SIZE[@]}; do
    printf "CHECKSUM\tCHECKSUM 2\tPACKET_SIZE\tCYCLES\n"
    for chksum in ${SWEEP_BUFFERS[@]}; do
        for chksumm in ${SWEEP_BUFFERS[@]}; do
            printf "$chksum\t$chksumm\t$pkt\t"
            make profile-run BENCHMARK=${BENCHMARK} EXTRA="-DREPEAT=200 -DPACKET_SIZE=${pkt} -DCHECKSUM_BUFFER_SIZE_2=${chksumm} -DCHECKSUM_BUFFER_SIZE_1=${chksum}" | grep cycles | rev | cut -d' ' -f1 | sed -e 's/[()]//g' | rev
        done
    done | tee "${OUTDIR}/$pkt.tsv"
done

cd ${CWD}
