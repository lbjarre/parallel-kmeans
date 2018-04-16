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

int * initSeeds(int K, int N)
{
  int * seeds;
  seeds = malloc(K * sizeof seeds);
  for (int k = 0; k < K; k++) {
    seeds[k] = round(rand() / (RAND_MAX / N));
  }
  return seeds;
}
double ** assignSeeds(double **data, int *seeds, int K, int dim_x){

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

int main(int argc, char **argv)
{
  int rank, size, tag, rc, P, p, displs;
  int *sendcounts, *dspls, *seeds;
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

  char *file_name;
  FILE *fp;

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
    printf("Call program with <path> <xdim> <xlen> <K>\n");
    exit(1);
  }

  if (p == 0) {
    data = read_csv(file_name, dim_x, len_x);
    printInfo(file_name, len_x, dim_x, k);
    printf("Start scatter\n");
  }

  sendcounts = partition(len_x, P);
  dspls = displacements(len_x, P);
  rec_buf = malloc(sendcounts[p] * sizeof rec_buf);
  seeds = initSeeds(k, len_x);
  m = assignSeeds(data, seeds, k, dim_x);

  MPI_Scatterv(&data, sendcounts, dspls, MPI_CHAR, &rec_buf, tag, MPI_CHAR, 0, MPI_COMM_WORLD);

  printf("Scatter complete\n");



  rc = MPI_Finalize();
  return 0;

}
