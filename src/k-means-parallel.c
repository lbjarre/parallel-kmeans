#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "k_means.h"
#include "k-means-parallel.h"

#define MAX_ITER 1000

double ** kMeansParallel(double **x, const int k, const int dim, const int len, const int p)
{
    double **m, **m_;
    int k_count[k];
    int iter = 0;
    int not_converged = 1;

    /*
        Initialization and allocation of the class means
    */

    m  = malloc(k * sizeof m);
    m_ = malloc(k * sizeof m);

    for (int i = 0; i < k; ++i) {
        m[i]  = malloc(dim * sizeof *m);
        m_[i] = calloc(dim, sizeof *m);
    }

    /*
        The first k processors decide the starting positions of the class means
        and communicates this to the rest of the processors
    */

    if (p < k) {
        int index = rand() / (RAND_MAX / len);
        for (int i = 0; i < dim; ++i) {
            m[p][i] = x[index][i];
        }
    }

    for (int i = 0; i < k; ++i) {
        MPI_Bcast(m[i], dim, MPI_DOUBLE, i, MPI_COMM_WORLD);
    }

    /*
        Main procedure loop
    */

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
                MPI_Allreduce(&m_[i][j], &m_[i][j], 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
            }

            MPI_Allreduce(&k_count[i], &k_count[i], 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

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

    return m;

}
