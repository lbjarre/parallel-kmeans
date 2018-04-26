#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "k_means.h"


int main(int argc, char **argv)
{
    int dim_x = 4;
    int len_x = 92;
    int k = 4;
    double **x, **m;
    char *file_name;

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
    printf("Running...\n");

    srand(1);

    x = read_csv(file_name, dim_x, len_x);

    m = k_means(x, dim_x, len_x, k);

    for (int i = 0; i < k; i++) {
        printf("Class %d\n", i);
        for (int j = 0; j < dim_x; ++j) {
            printf("%f\n", m[i][j]);
        }
    }

    return 0;
}
