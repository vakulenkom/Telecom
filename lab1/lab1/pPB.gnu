set terminal png
set output "pPB_SNR.png"

set logscale y
set format y "10^{%L}"

set grid ytics lc rgb "#bbbbbb" lw 1 lt 0
set grid xtics lc rgb "#bbbbbb" lw 1 lt 0

set ylabel "Error probability"
set xlabel "SNR"

plot '/Users/mike/Documents/Study/8semestr/Telecom/lab1/lab1/example.txt' using 1:2 title "Err probability per word-SNR" with lines
