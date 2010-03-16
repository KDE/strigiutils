set term png
set logscale x
set output 'x.png'
set xlabel 'exit time (s)'
set ylabel 'chance that exit time is larger'
set style line 1 lt 1 lw 6
unset key
plot 'graph'
