# Scale font and line width (dpi) by changing the size! It will always display stretched.
# set terminal svg size 400,300 enhanced fname 'arial'  fsize 10 butt solid
# set output 'out.svg'
set terminal png size 800,600
set output 'graph.png'


# Key means label...
set key inside top left
set xlabel 'Matrix size'
set ylabel 'Time (sec)'
set title 'Matrix Multiplication Speed on Wolfgang Cluster'
plot  "mmult.txt" using 1:2 title 'unoptimized' with linespoints, "mmult_simd.txt" using 1:2 title 'SIMD' with linespoints, "mmult_simd_O3.txt" using 1:2 title 'SIMD with -O3' with linespoints, "mmult_openMP.txt" using 1:2 title 'openMP' with linespoints, "mmult_mpi.txt" using 1:2 title 'MPI' with linespoints, "mmult_mpi_omp_rand.txt" using 1:2 title 'MPI with OMP' with linespoints
