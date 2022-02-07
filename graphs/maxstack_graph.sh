#!/usr/bin/gnuplot -persist
set title "Maximum Memory Cost"
set grid
set key left top
set xlabel "Output Length (bytes)"
set ylabel "Number of Elements"
set xrange [38:58]

plot "100-save-Brent" w lp,\
     "One-stack" w lp,\
     "100-stack" w lp