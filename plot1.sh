#gnuplot plot.plg --persist


gnuplot -e "
set terminal wxt size 1300,600;
plot 'plot1.data' using 1 title 's1' with line, \
     'plot1.data' using 2 title 's2' with line, \
     'plot1.data' using 3 title 's3' with line, \
     'plot1.data' using 4 title 's4' with line, \
     'plot1.data' using 5 title 's5' with line, \
     'plot1.data' using 6 title 's6' with line, \
     'plot1.data' using 7 title 's7' with line, \
     'plot1.data' using 8 title 's8' with line, \
     'plot1.data' using 9 title 's9' with line, \
     'plot1.data' using 10 title 's10' with line, \
     'plot1.data' using 11 title 's11' with line, \
     'plot1.data' using 12 title 's12' with line, \
     'plot1.data' using 13 title 's13' with line, \
     'plot1.data' using 14 title 's14' with line, \
     'plot1.data' using 15 title 's15' with line, \
     'plot1.data' using 16 title 's16' with line, \
     'plot1.data' using 17 title 's17' with line, \
     'plot1.data' using 18 title 's18' with line, \
     'plot1.data' using 19 title 's19' with line, \
     'plot1.data' using 20 title 's20' with line;
pause mouse close
"