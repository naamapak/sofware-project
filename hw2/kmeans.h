#ifndef KMEANS_H
#define KMEANS_H

int calculateKmeans(double ** finalCentroids, double ** dataPoints, int * initialCentroids, int vectorsAmount, int d, int k, double eps, int maxIter);

#endif