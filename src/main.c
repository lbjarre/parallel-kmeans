#include <stdlib.h>
#include <stdio.h>
#include <math.h>


double dist_sq(double *x, double *y, int len)
{
    double sum = 0;
    for (int i = 0; i < len; ++i) {
        sum += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sum;
}


int nearest_mean(double *x, double **means, int dim_x, int k)
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


double ** k_means(double **x, int dim_x, int len_x, int k)
{
    double **m, **m_next;
    int k_count[k];
    int curr_class;
    int not_converged = 1;
    int iter = 0;

    m = malloc(k * sizeof m);
    m_next = malloc(k * sizeof m_next);
    /*  Randomly initialize the initial class means
    */
    for (int i = 0; i < k; ++i) {
        m[i] = malloc(dim_x * sizeof *m);
        m_next[i] = calloc(dim_x, sizeof *m_next);
        for (int j = 0; j < dim_x; ++j) {
            m[i][j] = (double) rand() / RAND_MAX;
        }
    }

    do {
        /*  Classify all of the data points to the current class means
            x_class keeps track of which class each data point belongs to
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
                k_count[j] = 0; // Zero out the bins for next iteration
            }
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
    } while (not_converged && iter++ < 100);

    return m;
}


int main(int argc, char **agrv)
{
    int dim_x = 2;
    int len_x = 50;
    int k = 4;
    double **x, **m;

    x = malloc(len_x * sizeof x);
    
    srand(14);

    for (int i = 0; i < len_x; ++i) {
        x[i] = malloc(dim_x * sizeof x[i]);
        for (int j = 0; j < dim_x; ++j) {
            x[i][j] = (double) rand() / RAND_MAX;
        }
    }

    m = k_means(x, dim_x, len_x, k);

    for (int i = 0; i < k; i++) {
        printf("Class %d\n", i);
        for (int j = 0; j < dim_x; ++j) {
            printf("%f\n", m[i][j]);
        }
    }

    return 0;
}

