#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <Python.h>
#include "kmeansdefs.h"

#define EPSILON 0.001 /* global constant according to the assignment's defenition */
size_t data_size = 0;  /* global variable to hold the number of data lines */
size_t dimention;
char* ERROR_MESSAGE = "An Error Has Occured\n";

// module's function table
static PyMethodDef kMeans_FunctionsTable[] = {
    {
        "fit", // name exposed to Python
        fit, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        "Implements k-means clustering" // documentation
    }, {
        NULL, NULL, 0, NULL
    }
};

// modules definition
static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeansmodule",
    "Module for doing kmeans clustering.",
    -1,
    kMeans_FunctionsTable 
};

PyMODINIT_FUNC PyInit_kmeansmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}

double distance(datapoint* p, cluster* c){
    /* computes distance from datapoint to cluster. assumes equal lengths */
    double sum = 0;
    int i;
    if (p == NULL || c == NULL || p->vector == NULL || c->vector == NULL) { /* checks for edge cases */
            printf("%s\n", ERROR_MESSAGE);
        return MAXFLOAT;
    }
    if (p->vector_size != c->vector_size) { /* dimention mismatch*/
            printf("%s\n", ERROR_MESSAGE);
        return MAXFLOAT;
    }
    for(i = 0; i < p->vector_size; i++){ /* does the actual calculation according to the formula given */
        sum += pow(p->vector[i] - c->vector[i], 2);
    };
    return sqrt(sum);
}

void copy_vector(cluster* dest, const datapoint* src) {
    int i;
    /* used to create the initial clusters */
    dest->vector_size = src->vector_size;
    dest->vector = malloc(dest->vector_size * sizeof(double));
    if (!dest->vector) {
        perror("An Error Has Occured\n");
        exit(1);
    }

    for (i = 0; i < src->vector_size; ++i) {
        dest->vector[i] = src->vector[i];
    }
}

void _remove_point(cluster* c, datapoint* p){
    /* removes point p from the array in c, doesn't do anything to point p */
    int index = 0;
    int found = 0;
    int i;
    for(i = 0; i < c->cluster_size; i++){
        if (c->points[i]->vector == p->vector){  
            index = i;
            found = 1;
            break;
        }
    }
    if(!found){ return; }
    for(; index < c->cluster_size - 1; index ++){
        c->points[index] = c->points[index + 1];
    }
    if (c->cluster_size == 0) {
        free(c->points);
        c->points = NULL;
    }
    c->cluster_size--;
    c->points = realloc(c->points, c->cluster_size * sizeof(datapoint*));
    return;
}

void add_point(cluster* c, datapoint* p){ 
    /* add p to c's points and update p's centroid */
    if(p->centroid == c){
        return;
    }
    if(p->centroid){
        _remove_point(p->centroid, p);
    }
    c->points = realloc(c->points, (++c->cluster_size) * sizeof(datapoint*));
    if (!c->points) {
            printf("%s\n", ERROR_MESSAGE);
        exit(1);
    }
    c->points = realloc(c->points, c->cluster_size * sizeof(datapoint*));
    c->points[c->cluster_size - 1] = p;

    p->centroid = c;
    return;
}


void update_vector(cluster c){
    /* update c to be the average of its points (average for each cordinate) - as described in the assignment */
    int i;
    int j;
    double sum = 0.0;
    double euclidean = 0.0;
    double prev;
    
    if(c.cluster_size == 0 || !c.points){
        return;
    }

    for(i = 0; i < c.vector_size; i++){ /* goes through each dimention */
        prev = c.vector[i];
        sum = 0.0; /* resets sum for every seperate dimention */ 
        for(j = 0; j < c.cluster_size; j++){ /* goes through each of vector's value in the current dimention */
            sum += c.points[j]->vector[i];
        }
        c.vector[i] = sum / c.cluster_size; /* finds the average */
        euclidean += pow(prev - c.vector[i], 2);
    }
    c.diff_to_prev = sqrt(euclidean);
    return;
}

/* See if this actually works */
datapoint** _read_python_datapoints(PyObject* py_data){
    int i = 0;
    datapoint** data = malloc(sizeof(datapoint*) * data_size);
    for(i=0; i<data_size; i++){
        data[i] = malloc(sizeof(datapoint));
    }
      if (!data) {
            printf("%s\n", ERROR_MESSAGE);
        PyErr_NoMemory();
        return NULL;
    }

    for (Py_ssize_t i = 0; i < data_size; ++i) {
        PyObject* py_point = PyList_GetItem(py_data, i);  // Borrowed reference
        // Get .vector attribute
        PyObject* py_vector = PyObject_GetAttrString(py_point, "vector");
        if (!py_vector || !PyList_Check(py_vector)) {
            PyErr_SetString(PyExc_TypeError, ERROR_MESSAGE);
            free(data);
            return NULL;
        }
        Py_ssize_t vec_size = PyList_Size(py_vector);
        data[i]->vector_size = (int)vec_size;
        data[i]->vector = (double*)malloc(sizeof(double) * vec_size);
        if (!data[i]->vector) {
            PyErr_NoMemory();
            free(data);
            return NULL;
        }
        for (Py_ssize_t j = 0; j < vec_size; ++j) {
            PyObject* item = PyList_GetItem(py_vector, j);  // Borrowed reference
            double val = PyFloat_AsDouble(item);
            if (PyErr_Occurred()) {
                Py_DECREF(py_vector);
                free(data[i]->vector);
                free(data);
                return NULL;
            }
            data[i]->vector[j] = val;
        }
        Py_DECREF(py_vector);

        // Get .centroid attribute (optional; could be None)
        PyObject* py_centroid = PyObject_GetAttrString(py_point, "centroid");
        if (py_centroid == Py_None || !py_centroid) {
            data[i]->centroid = NULL;
        } else {
            // You might want to convert or wrap the centroid object here
            data[i]->centroid = (cluster*)py_centroid; // Placeholder cast
        }

        Py_XDECREF(py_centroid);
    }

    return data;
}

cluster* _read_python_clusters(PyObject* py_clusters, int k) {
    if (!PyList_Check(py_clusters)) {
        PyErr_SetString(PyExc_TypeError, ERROR_MESSAGE);
        return NULL;
    }

    cluster* clusters = malloc(k * sizeof(cluster));
    if (!clusters) {
        PyErr_NoMemory();
        return NULL;
    }

    for (int i = 0; i < k; i++) {
        PyObject* py_centroid = PyList_GetItem(py_clusters, i); // Borrowed reference
        if (!py_centroid) {
            PyErr_SetString(PyExc_IndexError, ERROR_MESSAGE);
            free(clusters);
            return NULL;
        }

        // centroid.vector
        PyObject* py_vector = PyObject_GetAttrString(py_centroid, "vector");
        if (!py_vector || !PyList_Check(py_vector)) {
            PyErr_SetString(PyExc_TypeError, ERROR_MESSAGE);
            free(clusters);
            return NULL;
        }

        int vector_size = (int)PyList_Size(py_vector);
        double* vector = malloc(vector_size * sizeof(double));
        if (!vector) {
            PyErr_NoMemory();
            free(clusters);
            return NULL;
        }

        for (int j = 0; j < vector_size; j++) {
            PyObject* item = PyList_GetItem(py_vector, j); // Borrowed reference
            double val = PyFloat_AsDouble(item);
            if (PyErr_Occurred()) {
                PyErr_SetString(PyExc_TypeError, ERROR_MESSAGE);
                free(vector);
                free(clusters);
                return NULL;
            }
            vector[j] = val;
        }
        Py_DECREF(py_vector);

        // centroid.points (could be None or list)
        datapoint** points = NULL;
        int cluster_size = 0;

        // Fill the cluster struct
        clusters[i].vector = vector;
        clusters[i].vector_size = vector_size;
        clusters[i].cluster_size = 0;
        clusters[i].points = malloc(sizeof(datapoint*)); // May be NULL
        clusters[i].diff_to_prev = 0.0;
        }

    return clusters;
}


PyObject* clusters_to_py_centroids(cluster* result, int k, PyObject* py_centroid_class, PyObject* py_datapoint_class) {
    PyObject* py_centroid_list = PyList_New(k);  // Final list to return
    if (!py_centroid_list) return NULL;

    for (int i = 0; i < k; i++) {
        cluster* c = &result[i];

        // Create Python list of floats for the centroid vector
        PyObject* vector_list = PyList_New(dimention);
        for (int j = 0; j < (int) dimention; j++) {
            PyObject* val = PyFloat_FromDouble(c->vector[j]);
            PyList_SET_ITEM(vector_list, j, val);  // Steals reference
        }

        // Build list of datapoint objects for .points
        PyObject* points_list = PyList_New(c->cluster_size);
        for (int k = 0; k < c->cluster_size; k++) {
            datapoint* dp = c->points[k];

            PyObject* dp_vector = PyList_New(dp->vector_size);
            for (int m = 0; m < dp->vector_size; m++) {
                PyObject* val = PyFloat_FromDouble(dp->vector[m]);
                PyList_SET_ITEM(dp_vector, m, val);
            }

            PyObject* dp_args = PyTuple_Pack(1, dp_vector);  // datapoint(vector)
            Py_DECREF(dp_vector);

            PyObject* dp_obj = PyObject_CallObject(py_datapoint_class, dp_args);
            Py_DECREF(dp_args);

            if (!dp_obj) {
                Py_DECREF(py_centroid_list);
                return NULL;
            }

            PyList_SET_ITEM(points_list, k, dp_obj);  // Steals reference
        }

        // Create centroid object: centroid(vector, points)
        PyObject* centroid_args = PyTuple_Pack(2, vector_list, points_list);
        Py_DECREF(vector_list);
        Py_DECREF(points_list);

        PyObject* centroid_obj = PyObject_CallObject(py_centroid_class, centroid_args);
        Py_DECREF(centroid_args);

        if (!centroid_obj) {
            Py_DECREF(py_centroid_list);
            return NULL;
        }

        PyList_SET_ITEM(py_centroid_list, i, centroid_obj);  // Steals reference
    }

    return py_centroid_list;
}


cluster* do_cluster(datapoint** data, cluster* result, int k, int iter, double epsilon){
    /* main algorithem that creates the clusters and doing the tuning iterations based on the algorithem from the assignment */
    cluster* curr_cent;
    double curr_delta;
    double loop_delta;
    int i, c_idx;
    size_t j_size;
    int converged=0;
/*
for(i = 0; i < k; i++){ // initialize k clusters
    copy_vector(&result[i], data[i]);
    result[i].points = malloc(sizeof(datapoint));
    result[i].points[0] = data[i];
    result[i].cluster_size = 1;
    data[i]->centroid = &result[i];
}*/
for(i = 0; i < iter; i++){
    converged = 1;
    for(j_size = 0; j_size < data_size; j_size++){
        curr_cent = data[j_size]->centroid;
        if(curr_cent == NULL || !curr_cent){
            curr_delta = MAXFLOAT;

        } else {
            curr_delta = distance(data[j_size], curr_cent);

        }
        for(c_idx = 0; c_idx < k; c_idx++){
            loop_delta = distance(data[j_size], &result[c_idx]);
            if(loop_delta < curr_delta){
                curr_delta = loop_delta;
                curr_cent = &result[c_idx];
            }
        }
        add_point(curr_cent, data[j_size]);
    }

    /* update the vectors */
    for(c_idx = 0; c_idx < k; c_idx++){
        update_vector(result[c_idx]);
        converged &= result[c_idx].diff_to_prev < EPSILON;
    }
}
return result;
}

void _free_datapoint(datapoint* point) {
    /*Not freeing clusters to not double free*/
    if (point == NULL) return;
    
    if (point->vector != NULL) {
        free(point->vector);
    }

}

void free_data(datapoint** data){
    size_t i;
    for(i=0 ; i < data_size; i++){
        free(data[i]->vector);
        free(data[i]);
    }
    free(data);
}

void free_clusters(cluster* clusters, int k) {
    int i;
    if (clusters == NULL) return;

    for(i=0; i < k; i++) {
        if (clusters[i].vector != NULL) {
            free(clusters[i].vector);
        }

        if (clusters[i].points != NULL) {
            free(clusters[i].points);
        }
    }

    free(clusters);
}

PyObject* fit(PyObject* self, PyObject* args){
    int k;
    int iter;
    PyObject* py_data;
    PyObject* py_clusters;
    PyObject* py_result;
    PyObject* py_centroid_class;
    PyObject* py_datapoint_class;
    datapoint** data;
    cluster* result;
    double epsilon;

    if(!PyArg_ParseTuple(args, "iiOOifOO", &k, &iter, &py_data, &py_clusters, &dimention, &epsilon, &py_centroid_class, &py_datapoint_class)){
        return NULL;
    }

    if (!(PyList_Check(py_data) && PyList_Check(py_clusters))) {
        PyErr_SetString(PyExc_TypeError, ERROR_MESSAGE);
        return NULL;
    }
    data_size = (int) PyList_Size(py_data);
    data = _read_python_datapoints(py_data);
    result = _read_python_clusters(py_clusters, k);
    // Parse python input: should be: a list of datapoints, 
    result = do_cluster(data, result, k, iter, epsilon);
    py_result = clusters_to_py_centroids(result, k, py_centroid_class, py_datapoint_class);
    free_clusters(result, k);
    free_data(data);
    return py_result;
}