#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


double ** read_csv(char *filepath, int dim_x, int len_x)
{
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

    return x;
}


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

    return m;
}


int main(int argc, char **argv)
{
    int dim_x = 4;
    int len_x = 92;
    int k = 4;
    double **x, **m;

    srand(1);

    char *file_name;
    FILE *fp;

    if (argc >= 2)
         file_name = argv[1];
         if (argc >= 3)
            dim_x = atoi(argv[2]);
            if (argc >= 4)
               len_x = atoi(argv[3]);
               if (argc >= 5)
                  k = atoi(argv[4]);

    else file_name = "data/iris.csv";
    printf("\n");
    printf("Data: %s\n",file_name);
    printf("Number of points: %d\n", len_x);
    printf("Number of dimensions: %d\n", dim_x);
    printf("Number of clusters: %d\n", k);
    printf("Running...\n");

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
