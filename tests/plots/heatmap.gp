#
# Various ways to create a 2D heat map from ascii data
#

# set terminal pdfcairo size 400,400 font 'Helvetica,10'
set terminal pdfcairo font "Gill Sans,14" size 7,7 linewidth 2 rounded fontscale 1.0

#set key autotitle columnhead
#unset key
set tic scale 0

if (!exists("mod1")) mod1='Mod 1'
if (!exists("mod2")) mod2='Mod 2'

# Color runs hot (black-red-yellow-white)
set palette rgbformula 21,22,23

set format x "2^{%L}"
set format y "2^{%L}"
set xlabel  mod1
set ylabel  mod2

# unset cbtics

set logscale xy 2

if (!exists("data")) data='64'


set title 'Latency \@'.data.'B'  offset 0,-1
set view map
set dgrid3d
set pm3d interpolate 0,0
set output 'graphs/'.data.'.pdf'
splot 'output/'.data.'.tsv' using 2:1:4 with pm3d title title
