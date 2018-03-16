#
# Various ways to create a 2D heat map from ascii data
#

#set terminal pdfcairo font "Gill Sans,9" size 8,8 linewidth 1 rounded fontscale 1.0
set terminal pngcairo font "Gill Sans,9" size 1024,1024 linewidth 1 rounded fontscale 1.0

#set key autotitle columnhead
#unset key
set tic scale 0

if (!exists("mod1")) mod1='Mod 1'
if (!exists("mod2")) mod2='Mod 2'

# Color runs hot (black-red-yellow-white)
set palette rgbformula 21,22,23

set format x "2^{%.0f}"
set format y "2^{%.0f}"
set xlabel  mod1
set ylabel  mod2

# unset cbtics

#set logscale xy 2

if (!exists("data")) data='64'

set output 'graphs/heatmap-multiplot.png'
set multiplot layout 2,2
set pm3d explicit
set pm3d corners2color c1
set view map
set dgrid3d
set pm3d scansforward

#set xtics (1,2,4,8,16,32,64,128,256,512,1024,2048,4096)
#set ytics (1,2,4,8,16,32,64,128,256,512,1024,2048,4096)
set xtics 0,1
set ytics 0,1

log2(x) = log(x) / log(2)

do for [i=1:words(files)] {
    set title 'Latency \@'.word(files, i).'B'  offset 0,-1
    set ylabel offset -1.5,0
    plot '<../plots/output-to-mat.sh '.'output/'.word(files, i).'.tsv' matrix using 1:2:3 with image title ''
} 

unset multiplot
