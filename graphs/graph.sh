#!/usr/bin/gnuplot -persist
set title "Function Calls Comparision"
set grid
set xlabel "Output length (bytes)"
set ylabel "Function Calls"
plot "floyd" w lp,\
     "brent" w lp,\
     "partition_brent" w lp,\
     "stack" w lp,\
     "partition_stack" w lp