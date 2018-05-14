#!/usr/bin/env bash

if ["$1" != ""]; then
  binfile="parallel"
else
  binfile="$1"
fi

dim="3"
k="10"
x_len="72248032"
p="2"
filename="data/image_data.csv"

# Kmeans: path - xdim - xlen - K
time mpirun -np $p bin/$binfile $filename $dim $x_len $k
