#ifndef _KMEANS
#define _KMEANS

double ** read_csv(char *, int, int);
double dist_sq(double *, double *, int);
int nearest_mean(double *, double **, int, int);
double ** k_means(double **, int, int, int);

#endif
