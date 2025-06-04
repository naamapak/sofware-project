#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EPSILON 0.001
size_t data_size = 0;  /* Global variable to hold the number of data lines */

/*typedef struct datapoint datapoint; DELETED THE LINES _ SHOWED ERROR CAUSE WE DEFINE AFTER: error: redefinition of typedef ‘datapoint’
typedef struct cluster cluster;*/

typedef struct datapoint {
    double* vector;
    int vector_size;
    struct cluster* centroid;
} datapoint;

typedef struct cluster {
    double* vector;
    int vector_size;
    int cluster_size;
    datapoint** points;
    double diff_to_prev;
} cluster;


double distance(datapoint* p, cluster* c){
    /* Computes distance from datapoint to cluster. Assumes equal lengths
    CHANGED THE UCLIDIAN FUNC _ WAS + INSTEAD OF - */
    double sum = 0;
    int i;
    if (p == NULL || c == NULL || p->vector == NULL || c->vector == NULL) {
        fprintf(stderr, "Null pointer in distance()\n");
        return MAXFLOAT;
    }
    if (p->vector_size != c->vector_size) {
        fprintf(stderr, "Mismatched vector sizes: p = %d, c = %d\n", p->vector_size, c->vector_size);
        return MAXFLOAT;
    }
    for(i = 0; i < p->vector_size; i++){
        sum += pow(p->vector[i] - c->vector[i], 2);
    };
    return sqrt(sum);
}

void copy_vector(cluster* dest, const datapoint* src) {
    int i;
    /* Used for creating the initial clusters */
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

datapoint** _read_input(){
    /* Reads input from stdin using getchar.
    not null-terminating anything, keeping track of size in data_size
    Keeps sizes of each line */
    size_t prev_len= 0;
    size_t line_len = 0;
    size_t len = 0;
    double curr_d;
    char c = getchar();
    datapoint* p;  /*MOVED THIS TO THE TOP BEFORE CODE*/ 
    datapoint** temp;
    char* current_num;
    double* line;
    datapoint** result = malloc(sizeof(datapoint));
    
    while(c == EOF || c == '\0' || c == ' '){
        c = getchar();
    }
    while(c != EOF){ /* Iterate over file */
        line_len = 0;
        line = malloc(sizeof(double));
        while(c != '\n'){ /* Iterate over line */
            len = 0;
            current_num = malloc(sizeof(char));
            if(!current_num){
                printf("An error has occured\n");
                exit(1);
            }
            while(c != ',' && c != '\n'){ /* Iterate over number */
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
            /* printf("reallocating line for size %ld with num %f\n",line_len, curr_d); */
            if(c == '\n'){
                break;
            }
            c = getchar();
        }

    if(data_size > 1 && line_len != prev_len){ /* Validates lines being the same length */
            printf("Invalid input\n"); /* TODO make this the correct */
            exit(1);
        }

    prev_len = line_len;
    p = malloc(sizeof(datapoint));
    p->vector = line;
    p->vector_size = line_len;
    p->centroid = NULL;
    /* result[data_size] = p;
    data_size++;
        CHANGED FROM EARLIER LINE: result = realloc(result, (data_size) * sizeof(datapoint)); - size of the pointer not the actual data_point
    printf("reallocating result: %ld \n", data_size);
    if(data_size > 1){
    result = realloc(result, sizeof(datapoint) * data_size);
    }
    Reallocate result array to hold one more datapoint */
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
    /* Removes p from the array in c, doesn't do anything to p */
    int index = 0;
    int found = 0;
    int i;
    for(i = 0; i < c->cluster_size; i++){
        /*if(&c->points[i] == &p){ */
        if (c->points[i]->vector == p->vector){  /*CHANGED FROM EARLIER LINE - COMPARING POINTERS */
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

/*CHANGED THE FUNC VARS TO EFFECT THE ACTUAL OBJ OUTSIDE OF THE FUNC
// CHANGED ALL THE . TO -> */
void add_point(cluster* c, datapoint* p){ /* void add_point(cluster c, datapoint p)
    Add p to c's points and update p's centroid */
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

    int i;
    int j;
    double sum = 0.0;
    double euclidean = 0.0;
    double prev;
    /* Update c to have be the average of its points */
    if(c.cluster_size == 0 || !c.points){
        return;
    }

    for(i = 0; i < c.vector_size; i++){
        prev = c.vector[i];
        sum = 0.0; /*ADDED THIS LINE TO RESET SUM EVERY DIM */ 
        for(j = 0; j < c.cluster_size; j++){
            sum += c.points[j]->vector[i];
        }
        c.vector[i] = sum / c.cluster_size;
        euclidean += pow(prev - c.vector[i], 2);
    }
    c.diff_to_prev = sqrt(euclidean);
    return;
}

cluster* do_cluster(datapoint** data, int k, int iter){
    cluster* result = malloc(sizeof(cluster) * k);
    cluster* curr_cent;
    double curr_delta;
    double loop_delta;
    int i, c_idx;
    size_t j_size;
    int j_int;
    int converged=0;
    double** prev_centroids;

for(i = 0; i < k; i++){ /* Initialize k clusters */
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
    int all_points = 0;
    /* update the vectors */
    for(c_idx = 0; c_idx < k; c_idx++){
        update_vector(result[c_idx]);
        converged &= result[c_idx].diff_to_prev < EPSILON;
        all_points += result[c_idx].cluster_size;
    }
    // /* check convergence */
    // for (c_idx = 0; c_idx < k; c_idx++) {
    //     double sum = 0.0;
    //     for (j_int = 0; j_int < result[c_idx].vector_size; j_int++) {
    //         sum += pow(prev_centroids[c_idx][j_int] - result[c_idx].vector[j_int], 2);
    //         /*if (fabs(prev_centroids[c_idx][j_int] - result[c_idx].vector[j_int]) >= EPSILON) {
    //             converged = 0;
    //             break;
    //         }*/
    //     }
    //     if(sqrt(sum) >= EPSILON){
    //         converged = 0;
    //         break;
    //     } else { printf("%f\n", sum); }
    //     if (!converged){
    //     break; }
    // }

    // /* free memory */
    // for (c_idx = 0; c_idx < k; c_idx++) {
    //     free(prev_centroids[c_idx]);
    // }
    // free(prev_centroids);
    // /* stop if converged */
    // if (converged) {
    //     break;
    // }
   

}
return result;
}

void free_data(datapoint** data){
    /* Freeing vectors and points. Not freeing clusters to not double-free */
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

/*void main(){
    int k;
    int iter;
    scanf("%d %d", &k, &iter);
    datapoint** data = _read_input(k);
    cluster* result = do_cluster(data, k, iter);
    free_data(data);
    free_clusters(result, k);
}
NEW MAIN _ INCLUDES INT MAIN AS REQUIRED ("return 0 / 1")*/
int main(int argc, char* argv[]) {

    int k, iter, i, j;
    datapoint** data;
    cluster* result;

    if (argc != 2 && argc != 3) { /*valid input is getting one argument k or 2 arguments k and iter, all else invalid */
        printf("Invalid input\n");
        return 1;
    }
    /* classify the inputs to k and iter */
    k = atoi(argv[1]);
    iter = (argc == 3) ? atoi(argv[2]) : 400; /*if iter is provided convert into int*/ 

    data = _read_input(); /* reads data points from stdin */

    if (k <= 1 || k >= (int)data_size) { /* checks validity of k */
        printf("Incorrect number of clusters!\n");
        free_data(data);
        return 1;
    }

    if (iter <= 1 || iter >= 1000) {/* checks validity of iter */
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