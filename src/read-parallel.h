#ifndef _READ_PARALLEL
#define _READ_PARALLEL

double ** read_parallel_csv(MPI_File, const int, const int, const int, const int, const int, const int);
double ** char_to_double_array(char*, const int, const int, const int);

#endif
