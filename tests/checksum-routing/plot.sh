#!/bin/bash

mkdir -p graphs/

CWD=$(pwd)
PIPELINE=$(echo $CWD | rev | cut -d'/' -f1 | rev)
MOD1=$(echo $PIPELINE | cut -d'-' -f1)
MOD2=$(echo $PIPELINE | cut -d'-' -f2)

files="64 256 512 1400"

for i in $files; do
    TITLE="$MOD1/$MOD2 at ${i}B"
    gnuplot -e "data='$i'" -e "title='$TITLE'" -e "mod1='$MOD1'" -e "mod2='$MOD2'" ../plots/heatmap.gp;
done

gnuplot -e "files='$files'" -e "mod1='$MOD1'" -e "mod2='$MOD2'" ../plots/heatmap-layout.gp;
rsync -r graphs/ friend@omid.io:/www/io-omid/graphs/nfv/$PIPELINE --delete 

echo "View files at: https://omid.io/graphs/nfv/$PIPELINE/"
