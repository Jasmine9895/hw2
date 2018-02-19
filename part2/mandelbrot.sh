#!/bin/bash
#$ -N Mandelbrot
#$ -q class8i
#$ -pe mpi 32
#$ -R y

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load openmpi-1.8.3/gcc-4.9.2

# Run the program 
mpirun -np 8  ./mandelbrot_serial 1000 1000

echo "Running Joe for 1"
./mandelbrot_joe 1000 1000
echo "Running Joe for i4"
mpirun -np 4 ./mandelbrot_joe 1000 1000
echo "Running Joe for 8"
mpirun -np 8 ./mandelbrot_joe 1000 1000
#echo "Running Joe for 16"
#mpirun -np 16 -R y ./mandelbrot_joe 1000 1000
