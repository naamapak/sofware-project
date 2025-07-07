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

datapoint** _read_python_datapoints(PyObject* py_data, int dimention, int datasize);
cluster* _read_python_clusters(PyObject* py_clusters, int k);
cluster* do_cluster(datapoint** data, cluster* result, int k, int iter, double epsilon);
PyObject* fit(PyObject* self, PyObject* args);