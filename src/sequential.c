#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "k_means.h"


int main(int argc, char **argv)
{
    int dim_x = 4;
    int len_x = 92;
    int k = 4;
    double **x, **m;
    char *file_name;
    time_t start_time;
    time_t time_after_read;
    time_t time_after_calc;


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

    printf("\n");
    printf("Data: %s\n",file_name);
    printf("Number of points: %d\n", len_x);
    printf("Number of dimensions: %d\n", dim_x);
    printf("Number of clusters: %d\n", k);
    printf("Running sequential...\n");

    srand(1);

    start_time = time(NULL);
    x = read_csv(file_name, dim_x, len_x);
    time_after_read = time(NULL);
    m = k_means(x, dim_x, len_x, k);
    time_after_calc = time(NULL);

    /*
    for (int i = 0; i < k; i++) {
        printf("Class %d\n", i);
        for (int j = 0; j < dim_x; ++j) {
            printf("%f\n", m[i][j]);
        }
    }
    */

    printf("Time for read file: %ld seconds \n", time_after_read-start_time);
    printf("Time for calc k-means: %ld seconds \n", time_after_calc - time_after_read);

    return 0;
}
