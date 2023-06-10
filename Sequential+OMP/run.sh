#! /bin/sh

g++-13 -g "sequential+omp.cpp" -fopenmp -o sequential+omp
./sequential+omp