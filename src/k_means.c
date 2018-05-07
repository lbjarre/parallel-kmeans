#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "k_means.h"

double ** read_csv(char *filepath, const int dim_x, const int len_x)
{
    const int MAX_LEN = 128;
    FILE *fp = fopen(filepath, "r");
    char line[MAX_LEN];
    char *token;
    double *x_line, **x;
    int c = 0, i = 0;

    x = malloc(len_x * sizeof x);

    while (fgets(line, MAX_LEN, fp) && i < len_x) {
        x_line = malloc(dim_x * sizeof x_line);
        token = strtok(line, ",");
        while (token) {
            x_line[c++] = strtod(token, NULL);
            token = strtok(NULL, ",");
        }
        c = 0;
        x[i++] = x_line;
    }

    return x;
}


double dist_sq(double *x, double *y, const int len)
{
    double sum = 0;
    for (int i = 0; i < len; ++i) {
        sum += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sum;
}


int nearest_mean(double *x, double **means, const int dim_x, const int k)
{
    double dist;
    double min = INFINITY;
    int i_min = -1;

    for (int i = 0; i < k; ++i) {
        dist = dist_sq(x, means[i], dim_x);
        if (dist < min) {
            min = dist;
            i_min = i;
        }
    }
    return i_min;
}


double ** k_means(double **x, const int dim_x, const int len_x, const int k)
{
    double **m, **m_next;
    int k_count[k];
    int curr_class;
    int not_converged = 1;
    int iter = 0;
    int rand_x;

    m = malloc(k * sizeof m);
    m_next = malloc(k * sizeof m_next);
    /*  Randomly initialize the initial class means
    */
    for (int i = 0; i < k; ++i) {
        m[i] = malloc(dim_x * sizeof *m);
        m_next[i] = calloc(dim_x, sizeof *m_next);
        k_count[i] = 0;

        rand_x = rand() / (RAND_MAX / len_x);
        for (int j = 0; j < dim_x; ++j) {
            m[i][j] = x[rand_x][j];
        }
    }

    do {
        /*  Classify all of the data points to the current class means
            k_count keeps track of the number of data points for each class
        */
        for (int j = 0; j < len_x; ++j) {
            curr_class = nearest_mean(x[j], m, dim_x, k);
            k_count[curr_class]++;
            for (int l = 0; l < dim_x; ++l) {
                m_next[curr_class][l] += x[j][l];
            }
        }

        /*  Divide by number of classified points to get the new mean
        */
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < dim_x; ++l) {
                m_next[j][l] /= (k_count[j] == 0) ? 1 : k_count[j]; // Guarding against 0 division
            }
            k_count[j] = 0; // Zero out the bins for next iteration
        }

        /*  Compute the exit condition
        */
        not_converged = 0;
        for (int j = 0; j < k; ++j) {
            for (int l = 0; l < dim_x; ++l) {
                if (m[j][l] != m_next[j][l]) {
                    not_converged = 1;
                }
                m[j][l] = m_next[j][l];
                m_next[j][l] = 0;
            }
        }
    } while (not_converged && iter++ < 1000);

    free(m_next);

    return m;
}


int * assign_nearest_cluster(double **x, double **m, const int k, const int dim, const int len)
{
  int i = 0;
  int *o;

  /*
  Loop through the points to assign the nearest cluster.
  Returns a list of each points nearest cluster.
  */
  o = malloc(len * sizeof o);
  for (i = 0; i < len; i++) {
    o[i] = nearest_mean(x[i], m, dim, k);
  }

  return o;
}
