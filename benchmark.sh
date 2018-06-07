#!/bin/bash

CWD=$(pwd)
SWEEP_BUFFERS=(1 2 4 8 16 32 64 128 256 512 1024)

OUTDIR=${CWD}/output
mkdir -p ${OUTDIR}

# cd ${CWD}/../..

#PIPELINE=$(echo $CWD | rev | cut -d'/' -f1 | rev)

PACKET_SIZE=1400
printf "checksum\trfile\tPACKET_SIZE\tCYCLES\n"
mod2=32
pkt=${PACKET_SIZE}
for mod1 in ${SWEEP_BUFFERS[@]}; do
    for mod2 in ${SWEEP_BUFFERS[@]}; do
        printf "$mod1\t$mod2\t$pkt\t"
        #printf "$mod1\t$pkt\t"
        >&2 echo "LOG STAT::$mod1\t$mod2\t$pkt\t"
        make clean && make rxer PROFILE=optimized EXTRA="-DMOD_BUFFER_SIZE_2=${mod2} -DMOD_BUFFER_SIZE_1=${mod1}" && sudo ./rxer -l 1-3 -n4 | grep cycles | rev | cut -d' ' -f1 | sed -e 's/[()]//g' | rev
    done
done | tee "${OUTDIR}/$pkt.tsv" 2>${OUTDIR}/$pkt.log

#cd ${CWD}
