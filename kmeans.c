#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EPSILON 0.001 /* global constant according to the assignment's defenition */
size_t data_size = 0;  /* global variable to hold the number of data lines */

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
        fprintf(stderr, "Null pointer in distance()\n");
        return MAXFLOAT;
    }
    if (p->vector_size != c->vector_size) { /* dimension mismatch*/
        fprintf(stderr, "Mismatched vector sizes: p = %d, c = %d\n", p->vector_size, c->vector_size);
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

/* returns 1 if s is a valid number */
int is_positive_int(const char *s)
{
    if (*s == '\0')  /* empty string */
        return 0;
    while (*s) {
        if (*s < '0' || *s > '9') /* non-digit */
            return 0;          
        ++s;
    }
    return 1;
}

datapoint** _read_input(){
    /* reads input from stdin using getchar.
    not null-terminating anything, keeping track of size in data_size, keeps sizes of each line */
    size_t prev_len= 0;
    size_t line_len = 0;
    size_t len = 0;
    double curr_d;
    char c = getchar();
    datapoint* p; 
    datapoint** temp;
    char* current_num;
    double* line;
    datapoint** result = malloc(sizeof(datapoint));
    
    while(c == EOF || c == '\0' || c == ' '){
        c = getchar();
    }
    while(c != EOF){ /* iterate over file */
        line_len = 0;
        line = malloc(sizeof(double));
        while(c != '\n'){ /* iterate over line */
            len = 0;
            current_num = malloc(sizeof(char));
            if(!current_num){
                printf("An error has occured\n");
                exit(1);
            }
            while(c != ',' && c != '\n'){ /* iterate over number */
                current_num = realloc(current_num, (++len) * sizeof(char));
                current_num[len - 1] = c;
                if(!current_num){
                    printf("An Error Has Occured\n");
                    exit(1);
                }
                c = getchar();
            }
            curr_d = strtod(current_num, NULL);
            free(current_num);
            line = realloc(line, (++line_len) * sizeof(curr_d));
            line[line_len - 1] = curr_d;

            if(c == '\n'){
                break;
            }
            c = getchar();
        }

    if(data_size > 1 && line_len != prev_len){ /* validates lines being the same length */
            printf("Invalid input\n"); /* TODO make this the correct */
            exit(1);
        }

    prev_len = line_len;
    p = malloc(sizeof(datapoint));
    p->vector = line;
    p->vector_size = line_len;
    p->centroid = NULL;
    temp = realloc(result, (data_size + 1) * sizeof(datapoint));
    if (!temp) {
        printf("An Error Has Occured\n");
        free(p->vector);/* DID IT: Also consider freeing p->vector if allocated earlier*/
        free(p);
        exit(1);
}
result = temp;
result[data_size] = p;
data_size++;

    c = getchar();
    }
    return result;
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
    if(c->cluster_size <= 1 || c->points == NULL){
        c->points = malloc(sizeof(datapoint*));
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

cluster* do_cluster(datapoint** data, int k, int iter){
    /* main algorithem that creates the clusters and doing the tuning iterations based on the algorithem from the assignment */
    cluster* result = malloc(sizeof(cluster) * k);
    cluster* curr_cent;
    double curr_delta;
    double loop_delta;
    int i, c_idx;
    size_t j_size;
    int j_int;
    int converged=0;
    double** prev_centroids;
    int all_points = 0;

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

    /* store previous centroids */
    prev_centroids = malloc(sizeof(double*) * k);
    for (c_idx = 0; c_idx < k; c_idx++) {
        prev_centroids[c_idx] = malloc(sizeof(double) * result[c_idx].vector_size);
        for (j_int = 0; j_int < result[c_idx].vector_size; j_int++) {
            prev_centroids[c_idx][j_int] = result[c_idx].vector[j_int];
        }
    }
    
    /* update the vectors */
    for(c_idx = 0; c_idx < k; c_idx++){
        update_vector(result[c_idx]);
        converged &= result[c_idx].diff_to_prev < EPSILON;
        all_points += result[c_idx].cluster_size;
    }
}
return result;
}

void free_data(datapoint** data){
    /* freeing vectors and points. Not freeing clusters to not double-free */
    int i;
    for(i = 0; i < (int) data_size; i++){
        free(data[i]->vector);
        free(data[i]);
    }
    free(data); 
    return;
}

void free_clusters(cluster* data, int k){
    int i;
    for (i = 0; i < k; i++) {
    free(data[i].vector);
    free(data[i].points);
    }
    free(data);
    return;
}


/* main function, it's int main because it suppose to return 0 / 1 */
int main(int argc, char* argv[]) {
    int k, iter, i, j;
    datapoint** data;
    cluster* result;

    if (argc != 2 && argc != 3) { /*valid input is getting one argument k or 2 arguments k and iter, all else invalid */
        printf("Invalid input\n");
        return 1;
    }
    if ( !is_positive_int(argv[1]) ||(argc == 3 && !is_positive_int(argv[2]))){/* checks if the numbers are valid */
    printf("Invalid input\n");
    return 1;
    }
    /* classify the inputs to k and iter */
    k = atoi(argv[1]);
    iter = (argc == 3) ? atoi(argv[2]) : 400; /* if iter is provided convert into int */ 

    data = _read_input(); /* reads data points from stdin */

    if (k <= 1 || k >= (int)data_size) { /* checks validity of k: 1 < k < N */
        printf("Incorrect number of clusters!\n");
        free_data(data);
        return 1;
    }

    if (iter <= 1 || iter >= 1000) {/* checks validity of iter: 1 < iter < 1000 */
        printf("Incorrect maximum iteration!\n");
        free_data(data);
        return 1;
    }
    result = do_cluster(data, k, iter);

    for (i = 0; i < k; i++) {
        for (j = 0; j < result[i].vector_size; j++) {
            if (j > 0) printf(",");
            printf("%.4f", result[i].vector[j]);
        }
        printf("\n");
    }

    free_data(data);
    free_clusters(result, k);
    return 0;
}