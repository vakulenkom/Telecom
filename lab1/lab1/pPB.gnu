set terminal png
set output "pPB_SNR all.png"

set logscale y
set format y "10^{%L}"

set grid ytics lc rgb "#bbbbbb" lw 1 lt 0
set grid xtics lc rgb "#bbbbbb" lw 1 lt 0

set ylabel "Error probability"
set xlabel "SNR"

plot '/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt' using 1:2 title "bit 1" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt' using 1:3 title "bit 2" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt' using 1:4 title "bit 3" with lines, '/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt' using 1:5 title "bit 4" with lines
