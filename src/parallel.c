#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

/* Helper functions from sequential */
#include "k_means.h"
#include "k-means-parallel.h"
#include "read-parallel.h"


void print_info(char * file_name, int len_x, int dim_x, int k){
  printf("\n");
  printf("Data: %s\n",file_name);
  printf("Number of points: %d\n", len_x);
  printf("Number of dimensions: %d\n", dim_x);
  printf("Number of clusters: %d\n", k);
  printf("Running parallel...\n");
}


void print_partition(double **x, int I, int dim_x){
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

  time_t start_time;
  time_t time_after_read;
  time_t time_after_calc;

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
    print_info(file_name, len_x, dim_x, k);
  }
  int overlap = 200;

  MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp);

  I = (len_x + P - p - 1) / P;

  printf("es before reading now\n");

  if (p==0) {start_time = time(NULL);}
  x = read_parallel_csv(fp, p, P, overlap, len_x, dim_x, I);

  if (p==0) {time_after_read = time(NULL);}
  double **m = k_means_parallel(x, k, dim_x, I, p, P);

  if (p==0) {
    time_after_calc = time(NULL);
    printf("Time for read file: %ld seconds \n", time_after_read-start_time);
    printf("Time for calc k-means: %ld seconds \n", time_after_calc - time_after_read);
  }

  /*
  for (int i = 0; i < k; ++i) {
    for (int j = 0; j < dim_x; ++j) {
      printf("p: %d, k: %d, k[%d]: %f\n", p, i, j, m[i][j]);
    }
  }

  int* closest_means;
  closest_means = assign_nearest_cluster(x, m, k, dim_x, I);

  const char* fname = "out.dat";
  printf("%d\n",I);
  //parallel_print_to_file(fname, closest_means, m, I, dim_x, k, p, P);
  */

  rc = MPI_Finalize();
  return 0;

}
