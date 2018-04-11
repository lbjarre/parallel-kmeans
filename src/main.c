#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double dist_sq(double *x, double *y, int len)
{
    double sum;
    for (int i = 0; i < len; ++i) {
        sum += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sum;
}

int nearest_mean(double *x, double **means, int len_x, int len_m)
{
    double dist;
    double min = INFINITY;
    int i_min = -1;
    
    for (int i = 0; i < len_m; ++i) {
        dist = dist_sq(x, means[i], len_x);
        printf("dist: %f\n", dist);
        if (dist < min) {
            min = dist;
            i_min = i;
        }
    }

    return i_min;
}

int main(int argc, char **agrv)
{
    int len = 5;
    int len_m = 3;
    double *x, *y, **m;

    x = malloc(len * sizeof x);
    y = malloc(len * sizeof y);
    
    srand(14);

    for (int i = 0; i < len; ++i) {
        x[i] = (double) rand() / RAND_MAX;
        y[i] = (double) rand() / RAND_MAX;
    }

    m = malloc(len_m * sizeof m);
    
    for (int i = 0; i < len_m; ++i) {
        m[i] = malloc(len * sizeof m);
        for (int j = 0; j < len; ++j) {
            m[i][j] = (double) rand() / RAND_MAX;
        }
    }

    double dist = dist_sq(x, y, len);
    printf("%f\n", dist);

    int i_x = nearest_mean(x, m, len, len_m);
    int i_y = nearest_mean(y, m, len, len_m);
    printf("x: %d, y: %d\n", i_x, i_y);

    free(x);
    free(y);
    free(m);

    return 0;
}

