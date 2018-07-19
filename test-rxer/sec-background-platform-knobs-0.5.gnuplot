load 'graph-incl/common.gnuplot'

set style fill solid
set boxwidth 0.9
set yrange [0:*]
set xrange [0.5:*]
unset key
set style data histogram
set xtics rotate by 0

set xtics offset 0,0 ("Naive" 1,"Batching" 2,"Data\nPref" 3,"Packet\nPref" 4, "Batch\nTuning" 5,"LinkOpt." 6,"Merging" 7)
set output "sec-background-platform-knobs.pdf"

set xlabel "Optimizations"
set ylabel "Throughput (Mpps)"

plot 'sec-background-platform-knobs-0.5.tsv' using 0:($4/1000000.0) with boxes lc 3, \
    '' using 0:($4/1000000.0):(($4-$10)/1000000.0):(($4+$10)/1000000.0) with yerrorbars lc rgb 'black' pt 0 lw 2

