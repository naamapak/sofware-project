#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double EPSILON = 0.001;
size_t data_size = 0;  // Global variable to hold the number of data lines

typedef struct datapoint datapoint;
typedef struct cluster cluster;

typedef struct datapoint {
    double* vector;
    int vector_size;
    struct cluster* centroid;
} datapoint;

typedef struct cluster {
    double* vector;
    int vector_size;
    int cluster_size;
    datapoint* points;
} cluster;


double distance(datapoint* p, cluster* c){
    // Computes distance from datapoint to cluster. Assumes equal lengths
    // CHANGED THE UCLIDIAN FUNC _ WAS + INSTEAD OF -
    double sum = 0;
    for(int i = 0; i < p->vector_size; i++){
        sum += pow(p->vector[i] - c->vector[i], 2);
    };
    return sqrt(sum);
}

void copy_vector(cluster* dest, const datapoint* src) { // Used for creating the initial clusters
    dest->vector_size = src->vector_size;
    dest->vector = malloc(dest->vector_size * sizeof(double));
    if (!dest->vector) {
        perror("An Error Has Occured\n");
        exit(1);
    }

    for (int i = 0; i < src->vector_size; ++i) {
        dest->vector[i] = src->vector[i];
    }
}

datapoint** _read_input(){
    /* Reads input from stdin using getchar.
    not null-terminating anything, keeping track of size in data_size
    Keeps sizes of each line */
    size_t prev_len;
    datapoint** result = malloc(sizeof(datapoint*));
    double curr_d;
    char c = getchar();
    while(c == EOF || c == '\0' || c == ' '){
        c = getchar();
    }
    while(c != EOF){ // Iterate over file
        size_t line_len = 0;
        double* line = malloc(sizeof(double));
        while(c != '\n'){ // Iterate over line
            size_t len = 0;
            char* current_num = malloc(sizeof(char));
            while(c != ',' && c != '\n'){ // Iterate over number
                current_num[len] = c;
                // printf("reallocating num\n");
                current_num = realloc(current_num, (++len) * sizeof(char));
                if(!current_num){
                    printf("An Error Has Occured\n");
                    exit(1);
                }
                c = getchar();
            }
            curr_d = strtod(current_num, NULL);
            free(current_num);
            line[line_len] = curr_d;
            // printf("reallocating line\n");
            line = realloc(line, (++line_len) * sizeof(double));
            if(c == '\n'){
                break;
            }
            c = getchar();
        }

    if(data_size > 1 && line_len != prev_len){ // Validates lines being the same length
            printf("Invalid input\n"); // TODO make this the correct 
            exit(1);
        }

    prev_len = line_len;
    datapoint* p = malloc(sizeof(datapoint));
    p->vector = line;
    p->vector_size = line_len;
    result[data_size] = p;
    data_size++;
    // CHANGED FROM EARLIER LINE: result = realloc(result, (data_size) * sizeof(datapoint)); - size of the pointer not the actual data_point
    // printf("reallocating result: %ld \n", data_size);
    if(data_size > 1){
    result = realloc(result, sizeof(datapoint) * data_size);
    }
    c = getchar();
    }
    return result;
}

void _remove_point(cluster* c, datapoint* p){
    /* Removes p from the array in c, doesn't do anything to p */
    int index = 0;
    for(int i = 0; i < c->cluster_size; i++){
        //if(&c->points[i] == &p){
        if (c->points[i].vector == p->vector){  //CHANGED FROM EARLIER LINE - COMPARING POINTERS

            index = i;
            break;
        }
    }
    for(index; index < c->cluster_size - 1; index ++){
        c->points[index] = c->points[index + 1];
    }
    if(c->cluster_size <= 1 || c->points == NULL){
        c->points = malloc(sizeof(datapoint*));
    } 
    c->points = realloc(c->points, c->cluster_size * sizeof(datapoint*));
    return;
}

// CHANGED THE FUNC VARS TO EFFECT THE ACTUAL OBJ OUTSIDE OF THE FUNC
// CHANGED ALL THE . TO -> 
void add_point(cluster* c, datapoint* p){ //void add_point(cluster c, datapoint p)
    /* Add p to c's points and update p's centroid */
    if(p->centroid == c){
        return;
    }
    if(p->centroid){
        _remove_point(p->centroid, p);
    }
    c->points = realloc(c->points, (++c->cluster_size) * sizeof(datapoint));
    if(!c->points){
        printf("An Error Has Occured\n");
        exit(1);
    }
    c->points[c->cluster_size - 1] = *p;
    p->centroid = c;
    return;
}

void update_vector(cluster c){
    /* Update c to have be the average of its points */
    if(c.cluster_size == 0 || !c.points){
        return;
    }
    double sum = 0;
    for(int i = 0; i < c.vector_size; i++){
        for(int j = 0; j < c.cluster_size; j++){
            sum += c.points[j].vector[i];
        }
        c.vector[i] = sum / c.cluster_size;
    }
    return;
}



cluster* do_cluster(datapoint** data, int k, int iter){
    cluster* result = malloc(sizeof(cluster) * k);
    cluster* curr_cent;
    double curr_delta;
    double loop_delta;
    for(int i = 0; i < k; i++){ // Initialize k clusters
        // CHANGED THIS: copy_vector(result[i].vector, data[i]->vector); - CAUSE WE NEED TO PASS cluster* AND data_point* as source NOT DOUBLE
        copy_vector(&result[i], data[i]); 
        result[i].points = malloc(sizeof(datapoint));
        result[i].points[0] = *data[i];
        result[i].cluster_size = 1;
    }
    for(int i = 0; i < iter; i++){
        int converged = 1;
        printf("Doint iteration %d\n", i);
        for(int j = 0; j < data_size; j++){
            // printf("Iterating over point %f, %f, %f\n", data[j]->vector[0], data[j]->vector[1], data[j]->vector[2]);
            datapoint* s = data[j];
            curr_cent = data[j]->centroid;
            if(curr_cent == NULL || !curr_cent){
                curr_delta = MAXFLOAT;
            } else {
                curr_delta = distance(data[j], curr_cent); // this has a segmentation fault that happens on line 85?
            }
            for(int c_idx = 0; c_idx < k; c_idx++){
                loop_delta = distance(data[j], &result[c_idx]);
                if(loop_delta < curr_delta){
                    curr_delta = loop_delta;
                    curr_cent = &result[c_idx];
                }

                // DELETED THE * ON BOTH curr_cent, data[j]
            }
            add_point(curr_cent, data[j]);
        }
        for(int c_idx = 0; c_idx < k; c_idx++){
            update_vector(result[c_idx]);
        }
    }
    return result;
}

void free_data(datapoint** data){
    /* Freeing vectors and points. Not freeing clusters to not double-free */
    for(int i = 0; i < (int) data_size; i++){
        free(data[i]->vector);
        free(data[i]);
    }
    free(data);
    return;
}

void free_clusters(cluster* data, int k){
    cluster c = data[0];
    for(int i = 0; i < k; i++){
        free(c.vector);
        free(c.points);
    }
    free(data);
    return;
}

//void main(){
//    int k;
//    int iter;
//    scanf("%d %d", &k, &iter);
//    datapoint** data = _read_input(k);
//   cluster* result = do_cluster(data, k, iter);
//    free_data(data);
//    free_clusters(result, k);
//}
//NEW MAIN _ INCLUDES INT MAIN AS REQUIRED ("return 0 / 1")
int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) { //valid input is getting one argument k or 2 arguments k and iter, all else invalid
        printf("Invalid input\n");
        return 1;
    }
    // classify the inputs to k and iter
    int k = atoi(argv[1]);
    int iter = (argc == 3) ? atoi(argv[2]) : 400; // if iter is provided convert into int

    datapoint** data = _read_input(); // reads data points from stdin

    if (k <= 1 || k >= data_size) { //checks validity of k
        printf("Incorrect number of clusters!\n");
        free_data(data);
        return 1;
    }

    if (iter <= 1 || iter >= 1000) {//checks validity of iter
        printf("Incorrect maximum iteration!\n");
        free_data(data);
        return 1;
    }
    cluster* result = do_cluster(data, k, iter);
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < result[i].vector_size; j++) {
            if (j > 0) printf(",");
            printf("%.4f", result[i].vector[j]);
        }
        printf("\n");
    }

    free_data(data);
    free_clusters(result, k);
    return 0;
}