# Kmeans: path - xdim - xlen - K
# time bin/main data/Phones_accelerometer.csv 6 13062476 2
mpicc src/parallel.c -o bin/parallel
# time mpirun bin/parallel data/Phones_accelerometer.csv 6 13062476 2 -np 2
time mpirun bin/parallel data/iris.csv 4 150 2 -np 2
