# Note you need gnuplot 4.4 for the pdfcairo terminal.

set terminal pdfcairo font "Gillius,10" linewidth 1 rounded fontscale 0.6 size 4.8, 2.2

# Line style for axes
set style line 80 lt rgb "#303030"

# Line style for grid
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"  # grey

set grid back linestyle 81
set border 3 back linestyle 80 # Remove border on top and right.  These
             # borders are useless and make it harder
             # to see plotted lines near the border.
    # Also, put it in grey; no need for so much emphasis on a border.
set xtics nomirror
set ytics nomirror

#set log x
#set mxtics 10    # Makes logscale look good.


colors = "#A00000 #00A000 #5060D0 #F25900 #B56015 #88B979 #FF3B00 #BC92AE #A19EB5"
#array pointtypes[8] = [4,6,8,10,12,3,1,2]

# Line styles: try to pick pleasing colors, rather
# than strictly primary colors or hard-to-see colors
# like gnuplot's default yellow.  Make the lines thick
# so they're easy to see in small plots in papers.
set for [i=1:8] style arrow i lc rgb word(colors, i) lw 1 nohead
set for [i=1:8] linetype i linewidth 2 lc rgb word(colors, i)

set style line 1 pt 1 ps 3
set style line 2 pt 6 ps 3
set style line 3 pt 2 ps 3
set style line 4 pt 9 ps 3
set style line 9 lt 2 lc "black" lw 5 ps 3

set ylabel offset 1.2,0,0 font "Gillius,12"
set xlabel offset 0.0,-1,0 font "Gillius,12"

ymax=45
