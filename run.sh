#!/usr/bin/env bash

if ["$1" != ""]; then
  binfile="parallel"
else
  binfile="$1"
fi

# Kmeans: path - xdim - xlen - K
time mpirun bin/$binfile data/iris_trim.csv 3 149 2 -np 2
