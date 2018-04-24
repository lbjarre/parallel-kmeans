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
    printf("%d: ",i+1);
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
  int sum, local_file_size, local_start, local_end, I, i, j, c, n;
  char *local_file_partition, *token, *line;
  double *x_line;
  double ** x;
  MPI_Offset file_size, global_end, global_start;

  //get total file size
  MPI_File_get_size(in, &file_size);
  file_size--;
  printf("Total file size: %d\n", file_size);

  //calculate local file size
  local_file_size = (file_size + P - p - 1) / P +1 ;

  //calculate absolute file start and top index
  global_start = 0;
  for (int i = 0; i < p; i++) {
    global_start = global_start + (file_size + P - i -1) / P;
  }
  global_end = global_start + local_file_size - 1;

  if (p != P-1) {
    local_file_size += overlap;
  }

  //allocate space for local part of file, specific for each process with
  local_file_partition = malloc(local_file_size * sizeof local_file_partition + 1);

  /*Read file in, from global_start, a total of local_file_size bytes.
   Save to local_file_partition */
  MPI_File_read_at_all(in, global_start, local_file_partition, local_file_size, MPI_CHAR, MPI_STATUS_IGNORE);
  local_file_partition[local_file_size] = '\n'; //set last index to NL. Will use this later.


  //Trim both file ends as the file most likely did not stop dividing on new line.
  local_start = 0;
  local_end = local_file_size ;
  printf("%d\n", local_start);
  printf("%d\n", local_end);

  /*

  if (p != 0) {
      while(local_file_partition[local_start] != '\n'){
        printf("%c", local_file_partition[local_start]);
        local_start++;
      }
      printf("\n");
      local_start++;
  }
  if (p != P-1){
      local_end-=overlap;
      while(local_file_partition[local_end] != '\n') {
        local_end++;
        printf("%c", local_file_partition[local_end]);
      }
      local_end++;
      printf("\n");
  }
  local_file_size = local_end - local_start + 1;
  */

  //convert str to char
  I = (len_x + P - p - 1) / P;
  x = malloc(I * sizeof x);
  line = malloc(overlap*2 * sizeof line);
  j = 0; i = 0; c = 0;
  /*if(p==1){
    printf("Local start: %d \nLocal end: %d\nLocal size: %d\n ", local_start, local_end, local_file_size);
  }*/
  for (n = local_start; n < local_end; n++) {
    if (local_file_partition[n] != '\n') {
      line[j++] = local_file_partition[n];
    }else{
      x_line = malloc(dim_x * sizeof x_line);
      token = strtok(line, ",");
      while(token) {
        x_line[c++] = strtod(token, NULL);
        token = strtok(NULL, ",");
      }
      c = 0; j = 0;
      x[i++] = x_line;
      free(line);
      line = malloc(overlap * 2 * sizeof line);
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
  int overlap = 100;
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
