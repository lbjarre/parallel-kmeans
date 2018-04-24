#!/usr/bin/env bash

# Kmeans: path - xdim - xlen - K
# time mpirun bin/read data/iris_trim.csv 3 150 2 -np 2
time mpirun bin/read data/data_trim.csv 5 13062475 2 -np 2
