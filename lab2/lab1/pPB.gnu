set terminal png
set output "pPB_SNR all.png"

set logscale y
set grid xtics ytics mytics
set mytics 10
set format y "10^{%L}"

set grid ytics lc rgb "#bbbbbb" lw 1 lt 0
set grid xtics lc rgb "#bbbbbb" lw 1 lt 0

set ylabel "Error probability"
set xlabel "SNR"

plot '/Users/mike/Documents/Study/8semestr/Telecom/lab2/lab1/example.txt' using 1:2 title "M = 2" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab2/lab1/example.txt' using 1:3 title "M = 4" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab2/lab1/example.txt' using 1:4 title "M = 8" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab2/lab1/example.txt' using 1:5 title "M = 16" with lines
