gnuplot -e "
plot 'plot2.data' using 1 title 'Summary Error' with points pointsize 0.5, \
     'plot2.data' using 2 title 'Output Error'  with points pointsize 0.5;
pause -1
"

