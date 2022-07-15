gnuplot -e "
set terminal wxt size 1300,600;
plot 'plot2.data' using 1 title 'Summary Error' with points pointsize 0.5, \
     'plot2.data' using 2 title 'Output Error'  with points pointsize 0.5;
pause mouse close
"

