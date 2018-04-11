#include <stdlib.h>
#include <stdio.h>

double dist_sq(double *x, double *y, int len)
{
    double sum;
    for (int i = 0; i < len; ++i) {
        sum += (x[i] - y[i]) * (x[i] - y[i]);
    }
    return sum;
}

int main(int argc, char **agrv)
{
    int len = 5;
    double *x, *y;

    x = malloc(len * sizeof x);
    y = malloc(len * sizeof y);
    
    srand(0);

    for (int i = 0; i < len; ++i) {
        x[i] = (double) rand() / RAND_MAX;
        y[i] = (double) rand() / RAND_MAX;
    }

    double dist = dist_sq(x, y, len);
    printf("%f\n", dist);

    free(x);
    free(y);

    return 0;
}

