import sys
import math

from typing import List

EPSILON: float = 0.001
DEFAULT_ITER = 200


class Centroid:
    vectors: List[List[float]]
    mean: List[float]
    d: int
    
    def __init__(self, vector: List[float]):
        self.d = len(vector)
        self.vectors = [vector]
        self.mean = vector
        
    def calc_mean(self) -> List[float]:
        vectors_sum = [0] * self.d
        
        for i in range(self.d):
            vectors_sum[i] = sum([x[i] for x in self.vectors])
                
        return [x/len(self.vectors) for x in vectors_sum]
            

def read_data_from_file(file_path: str) -> List[str]:
    lines = []

    with open(file_path, 'r') as f:
        lines = f.readlines()
    
    return lines


def split_lines_to_vectors(lines: List[str]) -> List[List[float]]:
    vectors = []
    for line in lines:
        vectors.append([float(x) for x in line.split(',')])
    return vectors


def initialize_centroids(vectors: List, k: int) -> List[Centroid]:
    centroids: List[Centroid] = []
    for i in range(k):
        centroids.append(Centroid(vectors[i]))
    return centroids


def euclidean_distance(x: List[float], y: List[float]) -> float:
    sum_distance: float = 0
    for i in range(len(x)):
        sum_distance += (x[i] - y[i]) ** 2
    
    return math.sqrt(sum_distance)


def add_vector_to_closest_cluster(centroids: List[Centroid], vector: List[float]):
    distances = {
        euclidean_distance(vector, centroid.mean): 
            centroid for centroid in centroids
    }
    
    closest_distance = min(distances.keys())
    
    # Add vector to closest centroid
    distances[closest_distance].vectors.append(vector)


def repeat_k_clustering(centroids: List[Centroid], vectors:List[List[float]], iter: int):
    
    for _ in range(iter):
        diffs = []
        
        # Empty vectors in each centroid
        for centroid in centroids:
            centroid.vectors = []
        
        for vector in vectors:
            add_vector_to_closest_cluster(centroids, vector)

        # Update mean in each centroid
        for centroid in centroids:
            new_mean = centroid.calc_mean()
            diffs.append(euclidean_distance(new_mean, centroid.mean))
            centroid.mean = new_mean
        
        # Check for convergence
        if (max(diffs) < EPSILON):
            return centroids

    return centroids


def print_centroids(centroids: List[Centroid]):
    for c in centroids:
        print(','.join([ "%.4f" % x for x in c.mean]))


def main():
    if len(sys.argv) not in [3, 4]:
        print("An Error Has Occurred")
        return -1

    try:
        if len(sys.argv) == 4:
            k, iter, input_file = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]
        
        # If iter isn't specified 
        else:
            k, input_file = int(sys.argv[1]), sys.argv[2]
            iter = DEFAULT_ITER
    
    except TypeError:
        print("An Error Has Occurred")
        return -1

    raw_input = read_data_from_file(input_file)
    vectors = split_lines_to_vectors(raw_input)
    
    if not(1 < k < len(vectors)):
        print("Invalid number of clusters!")
        return -1
    
    centroids: List[Centroid] = initialize_centroids(vectors, k)    
    repeat_k_clustering(centroids, vectors, iter)
    print_centroids(centroids)

if __name__ == "__main__":
    main()
