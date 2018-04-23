#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

/* Helper functions from sequential */
#include "../include/k_means.h"


void printInfo(char * file_name, int len_x, int dim_x, int k){
  printf("\n");
  printf("Data: %s\n",file_name);
  printf("Number of points: %d\n", len_x);
  printf("Number of dimensions: %d\n", dim_x);
  printf("Number of clusters: %d\n", k);
  printf("Running...\n");
}


int * partition(int N, int P)
{
  int *I;

  I = malloc(P * sizeof I);

  for (int i = 0; i < P; ++i) {
    I[i] = (N + P - i - 1) / P;
  }
  return I;
}


int * displacements(int N, int P)
{
  int s = 0;
  int *I;

  I = malloc(P * sizeof I);

  for (int i = 0; i < P; ++i){
    I[i] = s;
    s = s + (N + P - i - 1) / P;
  }
  return I;
}


void printPartition(double **x, int I, int dim_x){
  /*
  Function for debugging
  */
  for (int i = 0; i < I; i++) {
    for (int j = 0; j < dim_x; j++) {
        printf("%f, ", x[i][j]);
    }
    printf("\n");
  }
}


int * initSeeds(int K, int N)
{
  int * seeds;
  seeds = malloc(K * sizeof seeds);
  for (int k = 0; k < K; k++) {
    seeds[k] = round(rand() / (RAND_MAX / N));
  }
  return seeds;
}


double ** assignSeeds(double **data, int *seeds, int K, int dim_x)
{
  double ** clusters;
  clusters = malloc(K * sizeof clusters);

  for (int i = 0; i < dim_x; i++) {
    clusters[i] = malloc(dim_x * sizeof *clusters);
    for (int j = 0; j < dim_x; ++j) {
        clusters[i][j] = data[seeds[i]][j];
    }
  }
  return clusters;
}


double ** read_parallel_csv(MPI_File in, const int p, const int P, const int overlap, int len_x, int dim_x)
{
  /*
    Reads from file in parallel.
  */
  int sum, loc_file_size, I;
  char *line;
  char *file_part, *token;
  double *x_line;
  double ** x;

  MPI_Offset file_size;
  MPI_Offset global_end;
  MPI_Offset global_start;

  MPI_File_get_size(in, &file_size);
  loc_file_size = (file_size + P - p - 1) / P;

  global_start = 0;
  for (int i = 0; i < p; i++) {
    global_start = global_start + (file_size + P - i -1) / P;
  }
  global_end = global_start + loc_file_size - 1;

  if (p != P-1) {
    global_end += overlap;
  }

  file_part = malloc(loc_file_size * sizeof file_part);

  /*
  Reads loc_file_size bytes from file in, from file index global_start and
  saves file to file_part
  */
  MPI_File_read_at_all(in, global_start, file_part, loc_file_size, MPI_CHAR, MPI_STATUS_IGNORE);
  file_part[loc_file_size] = '\0';

  /*
    Files are trimmed such that every process receives the correct number of rows.
  */
  int loc_start = 0, loc_end = loc_file_size - 1;
  if (p != 0) {
      while(file_part[loc_start] != '\n') loc_start++;
      loc_start++;
  }
  if (p != P-1){
      loc_end-=overlap;
      while(file_part[loc_end] != '\n') loc_end++;
  }

  loc_file_size = loc_end - loc_start + 1;
  file_part[loc_end+1] = '\n';

  /* Converts str from file to float array */
  I = (len_x + P - p - 1) / P;
  x = malloc(I * sizeof x);
  line = malloc(overflow * sizeof line);

  int i = 0, c = 0, j = 0;

  for(int n = loc_start; n <= loc_end; ++n){
    if (file_part[n] != '\n') {
      line[j++] = file_part[n];
    } else {
      x_line = malloc(dim_x * sizeof x_line);
      token = strtok(line, ",");
      while(token) {
        x_line[c++] = strtod(token, NULL);
        token = strtok(NULL, ",");
      }
      c = 0;
      j = 0;
      x[i++] = x_line;
      free(line);
      line = malloc(overflow * sizeof line);
    }
  }
  return x;
}



int main(int argc, char **argv)
{
  int rank, tag, rc, displs, P, p, I;
  int *sendcounts, *offset, *seeds;
  double **data, **x, **m, **rec_buf;

  int dim_x = 4;
  int len_x = 92;
  int k = 4;

  /*Initialize MPI*/
  MPI_Status status;
  tag = 100;
  rc = MPI_Init(&argc, &argv);
  rc = MPI_Comm_size(MPI_COMM_WORLD, &P);
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &p);
  MPI_Info info;

  char *file_name;
  MPI_File fp;

  if (argc >= 2){
  file_name = argv[1];
    if (argc >= 3){
      dim_x = atoi(argv[2]);
      if (argc >= 4){
        len_x = atoi(argv[3]);
        if (argc >= 5){
        k = atoi(argv[4]);
        }
      }
    }
  } else {

    printf("Program terminated due to improper input parameter. \nCall program with <path> <xdim> <xlen> <K>\n");
    exit(1);
  }


  if (p == 0) {
    printInfo(file_name, len_x, dim_x, k);
  }
  int overlap = 200;
  offset = displacements(len_x, P);

  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);

  x = read_parallel_csv(fp, p, P, overlap, len_x, dim_x);

  I = (len_x + P - p - 1) / P;

  if (p == 1) {
    printPartition(x, I, dim_x);
  }

  /*
  sendcounts = partition(len_x, P);
  dspls = displacements(len_x, P);
  rec_buf = malloc(sendcounts[p] * sizeof rec_buf);
  seeds = initSeeds(k, len_x);
  m = assignSeeds(data, seeds, k, dim_x);

  //MPI_Scatterv(&data, sendcounts, dspls, MPI_CHAR, &rec_buf, tag, MPI_CHAR, 0, MPI_COMM_WORLD);

  printf("Scatter complete\n");
  */

  rc = MPI_Finalize();
  return 0;
}
