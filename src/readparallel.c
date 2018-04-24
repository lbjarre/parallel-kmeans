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
  printf("%d: ",1);
  for (int j = 0; j < dim_x; j++) {
      printf("%f, ", x[0][j]);
  }
  printf("\n");

  printf("%d: ",I);
  for (int j = 0; j < dim_x; j++) {
      printf("%f, ", x[I-1][j]);
  }
  printf("\n");

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


double ** char_to_double(char *local_file_partition, const int dim_x, const int I, const int x_cols)
{
  char *token, *line;
  double *x_line, **x;
  int c = 0, i;

  x = malloc(I * sizeof x);
  for (i = 0; i < I; i++) {
    line = malloc(x_cols * sizeof line);
    x_line = malloc(dim_x * sizeof x_line);
    for (int j = 0; j < x_cols; j++) {
      line[j] = local_file_partition[i*x_cols + j];
    }
    token = strtok(line, ",");
    while (token) {
        x_line[c++] = strtod(token, NULL);
        token = strtok(NULL, ",");
    }
    c = 0;
    x[i++] = x_line;
    free(line);
    free(x_line);
  }

  return x;
}


double ** read_parallel_csv(MPI_File in, const int p, const int P, const int overlap, int len_x, int dim_x, const int I)
{
  /*
    Reads from file in parallel.
  */
  int x_cols, i, local_file_size;
  char *local_file_partition, *buffer_file;
  double **x;
  MPI_Offset file_size, global_start;

  //get total file size
  MPI_File_get_size(in, &file_size);

  //reads the first lines in file to calculate how and what to read
  buffer_file = malloc(overlap * sizeof buffer_file);
  MPI_File_read_at_all(in, 0, buffer_file, overlap, MPI_CHAR, MPI_STATUS_IGNORE);

  x_cols = 0;
  while(buffer_file[x_cols] != '\n'){
    x_cols++;
  }
  x_cols++; //to account for new line

  local_file_size = I * x_cols;

  global_start = 0;
  for (i = 0; i < p; i++) {
    global_start += (len_x + P - i - 1) / P;
  }

  local_file_partition = malloc(local_file_size * sizeof local_file_partition);

  MPI_File_read_at_all(in, global_start*x_cols, local_file_partition, local_file_size, MPI_CHAR, MPI_STATUS_IGNORE);

  x = char_to_double(local_file_partition, dim_x, I, x_cols);

  return x;
}


int main(int argc, char **argv)
{
  int rc, P, p, I;
  double **x;

  int dim_x = 4;
  int len_x = 92;
  int k = 4;

  /*Initialize MPI*/
  //tag = 100;
  rc = MPI_Init(&argc, &argv);
  rc = MPI_Comm_size(MPI_COMM_WORLD, &P);
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &p);

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
    printf("START READ FILE\n");
  }
  int overlap = 200;

  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);

  I = (len_x + P - p - 1) / P;

  x = read_parallel_csv(fp, p, P, overlap, len_x, dim_x, I);


  if (p == 1) {
    //printPartition(x, I, dim_x);
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
