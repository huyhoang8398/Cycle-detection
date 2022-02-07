#!/usr/bin/gnuplot -persist
set title "Average Running Time of Algorithms"
set grid
set key left top
set xlabel "Output Length (bytes)"
set ylabel "Number of MD5 Evaluations"
set xrange [38:58]
set yrange [0:130000000]

plot "Floyd" w lp,\
     "Brent" w lp,\
     "100-save-Brent" w lp,\
     "One-stack" w lp,\
     "100-stack" w lp