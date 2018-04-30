#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>


double ** char_to_double(char *local_file_partition, const int dim_x, const int I, const int x_cols)
{
  char *token, *line;
  double *x_line, **x;
  int c = 0, i, j;

  x = malloc(I * sizeof x);
  for (i = 0; i < I; i++) {

    line = malloc(x_cols * sizeof line);
    x_line = malloc(dim_x * sizeof x_line);

    //read in the file partition line by line
    for (j = 0; j < x_cols-1; j++) {
      line[j] = local_file_partition[i*x_cols + j];
    }
    token = strtok(line, ",");
    while (token) {
        x_line[c++] = strtod(token, NULL);
        token = strtok(NULL, ",");
    }
    c = 0;
    x[i] = x_line;
    free(line);
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
