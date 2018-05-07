#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "k_means.h"
#include "k-means-parallel.h"

#define MAX_ITER 1000

#define min(x, y) ( (x < y) ? x : y )
#define max(x, y) ( (x > y) ? x : y )


void init_p_geq_k(double **m, double **x, const int k, const int dim, const int len, const int p)
{
    int index;
    
    if (p < k) {
        
        index = rand() / (RAND_MAX / len);
        
        for (int i = 0; i < dim; ++i) {
            m[p][i] = x[index][i];
        }
    
    }

    for (int i = 0; i < k; ++i) {
        MPI_Bcast(m[i], dim, MPI_DOUBLE, k, MPI_COMM_WORLD);
    }

}


void init_p_lt_k(double **m, double **x, const int k, const int dim, const int len, const int p, const int P)
{
    int L = (int) floor(k / P);
    int R = (int) k % P;
    int I = (int) (p + 1) * L + min(p + 1, R);

    for (int i = p * L + min(p, R); i < I; ++i) {
        
        int index = rand() / (RAND_MAX / len);
        
        for (int j = 0; j < dim; ++j) {
            m[i][j] = x[index][j];
        }
    
    }

    for (int i = 0; i < k; ++i) {
        
        int r = max( floor(i / (L + 1)), floor((i - R) / L) );
        MPI_Bcast(m[i], dim, MPI_DOUBLE, r, MPI_COMM_WORLD);
    
    }

}


double ** k_means_parallel(double **x, const int k, const int dim, const int len, const int p, const int P)
{
    double **m, **m_;
    int k_count[k];
    int iter = 0;
    int not_converged = 1;

    printf("es time to do shiet\n");

    /*
        Initialization and allocation of the class means
    */

    m  = malloc(k * sizeof m);
    m_ = malloc(k * sizeof m);

    for (int i = 0; i < k; ++i) {
        m[i]  = malloc(dim * sizeof *m);
        m_[i] = calloc(dim, sizeof *m);
    }

    if (P > k) {
        init_p_geq_k(m, x, k, dim, len, p);
    }
    else {
        init_p_lt_k(m, x, k, dim, len, p, P);
    }

    /*
        Main procedure loop
    */

    printf("whattup\n");

    int class;

    do {

        /*
            Classify all of the locally assigned data points
            and start adding up them to calculate the next class means
        */

        for (int i = 0; i < len; ++i) {

            class = nearest_mean(x[i], m, dim, k);
            k_count[class]++;

            for (int j = 0; j < dim; ++j) {
                m_[class][j] += x[i][j];
            }

        }

        /*
            Communication step for sending each processors classified points
        */

        for (int i = 0; i < k; ++i) {

            for (int j = 0; j < dim; ++j) {
                MPI_Allreduce(MPI_IN_PLACE, &m_[i][j], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            }

            MPI_Allreduce(MPI_IN_PLACE, &k_count[i], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

        }

        /*
            Calculating the new class means
        */

        for (int i = 0; i < k; ++i) {

            for (int j = 0; j < dim; ++j) {
                m_[i][j] /= (k_count[i] == 0) ? 1 : k_count[i];
            }

            k_count[i] = 0;

        }

        /*
            Compute the exit condition:
            check if m == m_ for all classes
        */

        not_converged = 0;

        for (int i = 0; i < k; ++i) {

            for (int j = 0; j < dim; ++j) {

                if (m[i][j] != m_[i][j]) not_converged = 1;
                m[i][j]  = m_[i][j];
                m_[i][j] = 0;

            }

        }

    } while (not_converged && iter++ < MAX_ITER);

    free(m_);

    return m;

}
