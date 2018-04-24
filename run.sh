#!/usr/bin/env bash

# Kmeans: path - xdim - xlen - K
# time bin/main data/Phones_accelerometer.csv 6 13062475 2
# time mpirun bin/parallel data/Phones_accelerometer.csv 6 13062475 2 -np 2
time mpirun bin/read data/iris_trim.csv 3 150 2 -np 2
