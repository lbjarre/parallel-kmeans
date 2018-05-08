#!/usr/bin/env bash

if ["$1" != ""]; then
  binfile="parallel"
else
  binfile="$1"
fi

# Kmeans: path - xdim - xlen - K
time mpirun -np 2 bin/$binfile data/image_data.csv 3 72248032 10
