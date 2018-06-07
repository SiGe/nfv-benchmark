#!/bin/bash

modules="routing_stanford measurement_large identity checksum"
sizes="1 4 16 64 256"

#modules="routing_stanford_2" #measurement_large identity checksum"
#sizes="1"
if [ "$#" -lt "1" ]; then
    cat <<USAGE
Usage: $0 [unique-packet-count] [zipf] [packet-size]
USAGE
    exit 1
fi

UNIQUE=$1
ZIPF=$2
PACKET_SIZE=$3


BASE="${UNIQUE}/${ZIPF}"
LOGS="${BASE}/logs"
RESULTS="${BASE}/results"

mkdir -p "${LOGS}"
mkdir -p "${RESULTS}"


for modOne in ${modules}; do
    for modTwo in ${modules}; do
        echo "[`date`] Running ${modOne} ${modTwo} experiments"
        for sizeOne in ${sizes}; do
            for sizeTwo in ${sizes}; do
                printf "%d\t%d\t" "$sizeOne" "$sizeTwo"
                ./make-pipeline.sh $modOne,$sizeOne $modTwo,$sizeTwo drop,32 >"${LOGS}/build-${modOne}-${modTwo}-${sizeOne}-${sizeTwo}-${PACKET_SIZE}.log" 2>&1
                if [ $? -ne 0 ]; then
                    printf "FAILED\n"
                    continue
                fi
                while : ; do
                    sudo ../bin/rxer-test -l1-3 -n4 >out.log 2>&1
                    if grep -Fxq "FATAL" out.log; then
                        continue
                    else
                        break
                    fi
                done
                cycles=$(cat out.log | tee "${LOGS}/run-${modOne}-${modTwo}-${sizeOne}-${sizeTwo}-${PACKET_SIZE}.log" | grep Percentile | awk '{out=out $3 "\t"} END {print out}')
                #| grep cycles | rev | cut -d' ' -f1 | sed -e 's/[()]//g' | rev)
                if [ $? -ne 0 ]; then
                    printf "FAILED\n"
                else
                    printf "%s\n" "$cycles"
                fi
            done
        done >"${RESULTS}/${modOne}-${modTwo}-${PACKET_SIZE}.csv"
    done
done
