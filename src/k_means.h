#ifndef _KMEANS
#define _KMEANS

double ** read_csv(char *, const int, const int);
double dist_sq(double *, double *, const int);
int nearest_mean(double *, double **, const int, const int);
double ** k_means(double **, const int, const int, const int);
int * assign_nearest_cluster(double **, double **, const int, const int, const int);

#endif
