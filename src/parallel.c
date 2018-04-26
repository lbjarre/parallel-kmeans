#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

/* Helper functions from sequential */
#include "k_means.h"
#include "k-means-parallel.h"
#include "read-parallel.h"


void printInfo(char * file_name, int len_x, int dim_x, int k){
  printf("\n");
  printf("Data: %s\n",file_name);
  printf("Number of points: %d\n", len_x);
  printf("Number of dimensions: %d\n", dim_x);
  printf("Number of clusters: %d\n", k);
  printf("Running...\n");
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


int main(int argc, char **argv)
{
  int tag, rc, P, p, I;
  double **x;

  int dim_x = 4;
  int len_x = 92;
  int k = 4;

  /*Initialize MPI*/

  tag = 100;
  rc = MPI_Init(&argc, &argv);
  rc = MPI_Comm_size(MPI_COMM_WORLD, &P);
  rc = MPI_Comm_rank(MPI_COMM_WORLD, &p);
  //MPI_Info info;
  //MPI_Status status;

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

  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);

  I = (len_x + P - p - 1) / P;

  x = read_parallel_csv(fp, p, P, overlap, len_x, dim_x, I);

  /*
  if (p == 1) {
    printPartition(x, I, dim_x);
  }
  */

  double **m = kMeansParallel(x, k, dim_x, I, p);

  for (int i = 0; i < k; ++i) {
    for (int j = 0; j < dim_x; ++j) {
      printf("p: %d, k: %d, k[%d]: %f\n", p, i, j, m[i][j]);
    }
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
