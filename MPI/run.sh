#! /bin/sh

mpicxx mpi.cpp -o mpi -std=c++11
mpiexec -n 8 ./mpi