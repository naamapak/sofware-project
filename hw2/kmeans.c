#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kmeans.h"

#define MAX_DOUBLE 1.7976931348623157e308

typedef struct c{
    double **vectors;
    int vecCount;
    double *mean;
} Centroid;

void printGeneralError() {
    printf("An Error Has Occurred\n");
}

void freeMemory(double **vectors, int vectorsAmount, Centroid * centroids, int k){
    /* Free all data from vectors and all means stored in centroids */
    int i;

    for(i = 0; i < vectorsAmount; i++){
        //free(vectors[i]);
    }

    //free(vectors);

    for (i = 0; i < k; i++){
        //free(centroids[i].mean);
        if (centroids[i].vecCount != 0) {
            //free(centroids[i].vectors);
        }
    }

    //free(centroids);
}

int initializeCentroids(Centroid *centroid, int k, double **vectors, int d, int * initialCentroids){
    int i, j;

    /* Deepcopy of first k vectors */
    for (i = 0; i < k; i++){
        centroid[i].mean = malloc(sizeof(double) * d);
        if (centroid[i].mean == NULL) {
            return i;
        }
        for (j = 0; j < d; j++){
            centroid[i].mean[j] = vectors[initialCentroids[i]][j];
        }
        centroid[i].vecCount = 0;
    }
    return k;
}

double euclidianDistance(double *x, double *y, int d){
    int i = 0;
    double distanceSum = 0;

    for (i = 0; i < d; i++){
        distanceSum += pow((x[i] - y[i]), 2);
    }

    return sqrt(distanceSum);
}

int addToClosestCentroid(Centroid *centroids, int k, double *vector, int d){
    int i = 0;
    Centroid * closestCentroid;
    int minDistanceIndex = 0;
    double minDistance = MAX_DOUBLE;
    double currDistance = 0;
    double **vectorsTmp;

    /* Find matching centroid, mark it by index for later addition */
    for (i = 0; i < k; i++){
        currDistance = euclidianDistance(vector, centroids[i].mean, d);
        if (currDistance < minDistance){
            minDistance = currDistance;
            minDistanceIndex = i;
        }
    }

    /* Add as first vector in centroid */
    closestCentroid = &centroids[minDistanceIndex];

    if (closestCentroid->vecCount == 0) {
        closestCentroid->vectors = malloc(sizeof(double *));
        if (closestCentroid->vectors == NULL) {
            return -1;
        }
        closestCentroid->vectors[0] = vector;
        closestCentroid->vecCount++;
    }
    else {
        closestCentroid->vecCount++;
        vectorsTmp = realloc(closestCentroid->vectors, closestCentroid->vecCount * sizeof(double *));
        if (vectorsTmp == NULL) {
            return -1;
        }
        closestCentroid->vectors = vectorsTmp;
        closestCentroid->vectors[closestCentroid->vecCount - 1] = vector;
    }

    return 0;
}

void calcMean(double *mean, Centroid cent, int d){
    /* Calc new mean based on vector in this centroid and return it */
    int i, j;

    /* Ensure the mean starts from zero */
    for (i = 0; i < d; i++){
         mean[i] = 0;
    }

    for (i = 0; i < d; i++){
        /* Sum all coordinates */
        for(j = 0; j < cent.vecCount; j++){
            mean[i] += cent.vectors[j][i];
        }
        /* Divide by the number of vectors */
        if (cent.vecCount > 0) {
            mean[i] = mean[i] / cent.vecCount;
        }
    }
}

int doIter(Centroid * centroids, double ** vectors, int vectorsAmount, int d, int k, double eps) {
    int i;
    double *newMean;
    double distance;
    int result;

    /* Acts as boolean */
    int convergence;

    for (i = 0; i< vectorsAmount; i++) {
        result = addToClosestCentroid(centroids, k, vectors[i], d);
        if (result == -1) {
            printGeneralError();
            //freeMemory(vectors, vectorsAmount, centroids, k);
            exit(EXIT_FAILURE);
        }
    }

    convergence = 1;

    /* Iterate on all vectors, re-calculate new mean and check convergence */
    for (i = 0; i < k; i++){
        newMean = malloc (sizeof(double) * d);
        if (newMean == NULL) {
            printGeneralError();
            //freeMemory(vectors, vectorsAmount, centroids, k);
            exit(EXIT_FAILURE);
        }
        calcMean(newMean, centroids[i], d);

        /* Check if converged */
        distance = euclidianDistance(newMean, centroids[i].mean, d);
        if (distance >= eps){
            convergence = 0;
        }

        /* Update centroid's mean */
        //free(centroids[i].mean);
        centroids[i].mean = newMean;
    }

    /* Empty centroid's verctors */
    for (i = 0; i < k; i++) {
        if (centroids[i].vecCount > 0) {
            //free(centroids[i].vectors);
            centroids[i].vecCount = 0;
        }
    }

    return convergence;
}

void copyVectorsAfterFinish(double ** finalCentroids, Centroid * centroids, int k, int d){
    int i, j;

    for (i = 0; i < k; i++) {
        for (j = 1; j < d; j++) {
            finalCentroids[i][j] = centroids[i].mean[0];;
        }
    }
}

int calculateKmeans(double ** finalCentroids, double ** vectors, int * initialCentroids, int vectorsAmount, int d, int k, double eps, int iter) {
    /* Run until convergence or MAX_ITER was reached */
    int converged, result;
    Centroid * centroids;

    /* Allocate and initialize centroids */
    centroids = malloc(sizeof(Centroid) * k);
    if (centroids == NULL) {
        printGeneralError();
        //freeMemory(vectors, vectorsAmount, centroids, k);
        exit(EXIT_FAILURE);
    }
    result = initializeCentroids(centroids, k, vectors, d, initialCentroids);
    if (result != k) {
        printGeneralError();
        //freeMemory(vectors, vectorsAmount, centroids, k);
        exit(EXIT_FAILURE);
    }

    /* Run until convergence or iter was reached */ 
    converged = 0;
    while (iter > 0 && !converged){
        converged = doIter(centroids, vectors, vectorsAmount, d, k, eps);    
        iter--;
    }

    /* move result to  final dest */
    copyVectorsAfterFinish(finalCentroids, centroids, k, d);
    
    /* Free all remaining memory */
    //freeMemory(vectors, vectorsAmount, centroids, k);
    
    return 0;
}
