#define _GNU_SOURCE
#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <Python.h>

#define EPSILON 0.001 /* global constant according to the assignment's defenition */
size_t data_size = 0;  /* global variable to hold the number of data lines */

static PyObject* GetList(PyObject* self, PyObject* args)  // copy-pasted from course example
{
    int N,r;
    PyObject* python_val;
    PyObject* python_int;
    if (!PyArg_ParseTuple(args, "i", &N)) {
        return NULL;
    }
    python_val = PyList_New(N);
    for (int i = 0; i < N; ++i)
    {
        r = i;
        python_int = Py_BuildValue("i", r);
        PyList_SetItem(python_val, i, python_int);
    }
    return python_val;
}

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
    kMeans_FunctionsTable // Figure out how the hell this is done
};


PyObject* fit(PyObject* self, PyObject* args){
    int k;
    int iter;
    PyObject* py_data;
    PyObject* py_clusters;
    PyObject* py_result;
    size_t datasize;
    size_t dimention;
    datapoint** data;
    cluster* result;
    double epsilon;

    if(!PyArg_ParseTuple(args, "some string", &k, &iter, &py_data, &py_clusters, &datasize, &dimention, &epsilon)){
        return NULL;
    }

    if (!(PyList_Check(py_data) && PyList_Check(py_clusters))) {
        PyErr_SetString(PyExc_TypeError, "Expected data and clusters to be lists.");
        return NULL;
    }    

    data = _read_python_datapoints(py_data, dimention, datasize);
    result = _read_python_clusters(py_clusters, dimention, k);
    // Parse python input: should be: a list of datapoints, 
    result = do_cluster(data, result, k, iter, epsilon);
    free_clusters(result, k);
    free_data(data);
    return py_result;
}

/* represents a single data point in the data set */
typedef struct datapoint { 
    double* vector; /* cordinates of the point */
    int vector_size; /* dimension of the vector */
    struct cluster* centroid; /* pointer to the cluster the point is currently assigned to */
} datapoint;

typedef struct cluster {
    double* vector; /* cordinates of the cluster's centroid */
    int vector_size;/* dimension of the cluster's centroid vector */
    int cluster_size;/* number of points currently assigned to the cluster */
    datapoint** points;/* array of pointers to the datapoints in this cluster */
    double diff_to_prev;/* distance between current and previous centroid (to check convergence) */
} cluster;


double distance(datapoint* p, cluster* c){
    /* computes distance from datapoint to cluster. assumes equal lengths */
    double sum = 0;
    int i;
    if (p == NULL || c == NULL || p->vector == NULL || c->vector == NULL) { /* checks for edge cases */
        printf("An Error Has Occured\n");
        return MAXFLOAT;
    }
    if (p->vector_size != c->vector_size) { /* dimension mismatch*/
        printf("An Error Has Occured\n");
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
        printf("An Error Has Occurred\n");
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

    for(i = 0; i < c.vector_size; i++){ /* goes through each dimension */
        prev = c.vector[i];
        sum = 0.0; /* resets sum for every seperate dimension */ 
        for(j = 0; j < c.cluster_size; j++){ /* goes through each of vector's value in the current dimension */
            sum += c.points[j]->vector[i];
        }
        c.vector[i] = sum / c.cluster_size; /* finds the average */
        euclidean += pow(prev - c.vector[i], 2);
    }
    c.diff_to_prev = sqrt(euclidean);
    return;
}

datapoint** _read_python_datapoints(PyObject* py_data, int dimention, int datasize){
    datapoint* data = malloc(sizeof(datapoint) * datasize);
      if (!data) {
        PyErr_NoMemory();
        return NULL;
    }

    for (Py_ssize_t i = 0; i < datasize; ++i) {
        PyObject* py_point = PyList_GetItem(py_data, i);  // Borrowed reference

        // Get .vector attribute
        PyObject* py_vector = PyObject_GetAttrString(py_point, "vector");
        if (!py_vector || !PyList_Check(py_vector)) {
            PyErr_SetString(PyExc_TypeError, "Expected 'vector' to be a list");
            free(data);
            return NULL;
        }

        Py_ssize_t vec_size = PyList_Size(py_vector);
        data[i].vector_size = (int)vec_size;
        data[i].vector = (double*)malloc(sizeof(double) * vec_size);
        if (!data[i].vector) {
            PyErr_NoMemory();
            free(data);
            return NULL;
        }

        for (Py_ssize_t j = 0; j < vec_size; ++j) {
            PyObject* item = PyList_GetItem(py_vector, j);  // Borrowed reference
            double val = PyFloat_AsDouble(item);
            if (PyErr_Occurred()) {
                Py_DECREF(py_vector);
                free(data[i].vector);
                free(data);
                return NULL;
            }
            data[i].vector[j] = val;
        }
        Py_DECREF(py_vector);

        // Get .centroid attribute (optional; could be None)
        PyObject* py_centroid = PyObject_GetAttrString(py_point, "centroid");
        if (py_centroid == Py_None || !py_centroid) {
            data[i].centroid = NULL;
        } else {
            // You might want to convert or wrap the centroid object here
            data[i].centroid = (cluster*)py_centroid; // Placeholder cast
        }

        Py_XDECREF(py_centroid);
    }

    return &data;
}

cluster* do_cluster(datapoint** data, cluster* result, int k, int iter, double epsilon){
    /* main algorithem that creates the clusters and doing the tuning iterations based on the algorithem from the assignment */
    cluster* curr_cent;
    double curr_delta;
    double loop_delta;
    int i, c_idx;
    size_t j_size;
    int converged=0;

for(i = 0; i < k; i++){ /* initialize k clusters */
    copy_vector(&result[i], data[i]);
    result[i].points = malloc(sizeof(datapoint));
    result[i].points[0] = data[i];
    result[i].cluster_size = 1;
    data[i]->centroid = &result[i];
}

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