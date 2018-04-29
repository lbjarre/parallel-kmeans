#!/usr/bin/env bash

if ["$1" != ""]; then
  binfile="parallel"
else
  binfile="$1"
fi

# Kmeans: path - xdim - xlen - K
time mpirun bin/$binfile data/data_trim.csv 5 13062475 2 -np 2
