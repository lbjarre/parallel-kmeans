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

double ** read_parallel_csv(MPI_File *in, const int p, const int P, int *offset, const int overlap)
{
  char *file_part;
  int sum;
  int my_file_size;
  double ** x;

  MPI_Offset file_size;
  MPI_Offset global_end;
  MPI_Offset global_start;
  printf("bajs\n");

  MPI_File_get_size(*in, &file_size);
  file_size--; //remove EOF.
  my_file_size = (file_size + P - p - 1) / P; //partition size

  global_start = 0;
  for (int i = 0; i < p-1; i++) {
    global_start = global_start + offset[i];
  }
  global_end = global_start + my_file_size - 1;

  printf("global start: %lli\n",global_start);
  printf("global end: %lli\n",global_end);


  /*
  if (p == P-1) global_end = file_size-1;

  if (p != P-1) {
    global_end += overlap;
  }

  file_part = malloc(my_file_size * sizeof file_part);

  MPI_File_read_at_all(*in, global_start, file_part, my_file_size, MPI_CHAR, MPI_STATUS_IGNORE);
  file_part[my_file_size] = '\0';


    const int MAX_LEN = 128;
    FILE *fp = fopen(filepath, "r");
    char line[MAX_LEN];
    char *token;
    double *x_line, **x;
    int c = 0, i = 0;

    x = malloc(len_x * sizeof x);

    while (fgets(line, MAX_LEN, fp)) {
        x_line = malloc(dim_x * sizeof x_line);
        token = strtok(line, ",");
        while (token) {
            x_line[c++] = strtod(token, NULL);
            token = strtok(NULL, ",");
        }
        c = 0;
        x[i++] = x_line;
    }

    return x;*/

  return x;


}

int main(int argc, char **argv)
{
  int rank, tag, rc, displs, P, p;
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
    printf("Call program with <path> <xdim> <xlen> <K>\n");
    exit(1);
  }


  if (p == 0) {
    printInfo(file_name, len_x, dim_x, k);
  }
  int overlap = 50;
  offset = displacements(len_x, P);

  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);

  //read_parallel_csv(fp, p, P, offset, overlap);

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
