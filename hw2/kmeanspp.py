import numpy as np
import pandas as pd
import kmeansmodule as mykmeanssp # TODO rename stuff
import math
import sys

#do c api things

MAX_DISTANCE = 999999999999999

class centroid:
    ## changed from set to list
    def __init__(self, vector, points =None):
        self.vector = vector
        self.points = points if points is not None else []
        self.dimention = len(vector)
    
    def update_vector(self):
        if len(self.points) == 0:
            return
        for i in range(self.dimention):
            self.vector[i] = sum([p.vector[i] for p in self.points]) / len(self.points)
        return
    
    ##changed cause we are using a list not a set
    def add_point(self, p):
        if p.centroid:
            try:
                p.centroid.points.remove(p)
            except ValueError: # in case point is not actually there
                pass

        p.centroid = self
        self.points.append(p)
        return

class datapoint:
    def __init__(self, vector, centroid = None):
        self.vector = vector
        self.centroid = centroid
        self.dimention = len(vector)
        self.curr_distance = -1

    def distance(self, other):
        if len(other.vector) != len(self.vector):
            print("Unequal vector lengths")
            return -1
        else:
            return math.sqrt(sum([(self.vector[i] - other.vector[i]) ** 2 for i in range(self.dimention)]))    

def parse_input(file1, file2):
    df1 = pd.read_csv(file1, index_col=0, header=None)
    df2 = pd.read_csv(file2, index_col=0, header=None)    
    df = df1.join(df2, on=0, how='inner', lsuffix="_", sort=True)
    datapoints = [datapoint(list(row)) for row in df.values if len(row) > 0]
    return datapoints, df

## did a whole new func (based on prev logic but better)
def choose_centroids(data, k):
    np.random.seed(1234)
    centroids = []
    indices = range(len(data))
    chosen_i = []

    # Step 1: choose first centroid randomly
    first_i = np.random.choice(indices)
    first = data[first_i]
    centroids.append(first)
    chosen_i.append(first_i)

    for p in data:
        p.curr_distance = p.distance(first)

    for _ in range(1, k):
        distances = np.array([p.curr_distance ** 2 for p in data])
        probs = distances / distances.sum()
        chosen_i.append(np.random.choice(indices, p=probs))
        chosen = centroid(data[chosen_i[-1]].vector)

        centroids.append(chosen)

        # Update distances
        for p in data:
            dist = p.distance(chosen)
            if dist < p.curr_distance:
                p.curr_distance = dist

    return centroids, chosen_i

def parse_args():
    args = sys.argv
    if len(args) not in [5,6]:
        print("Invalid number of arguments!")
        exit(1)
    k = int(args[1])
    iter = 300 if len(args) == 5 else int(args[2])
    eps = float(args[-3])
    file1, file2 = args[-2], args[-1]
    return k, iter, eps, file1, file2

if __name__ == "__main__":
    k, iter, eps, file1, file2 = parse_args()
    data, df = parse_input(file1, file2)
    centroids, indices = choose_centroids(data, k)

    initial_centroids = [c.vector for c in centroids]
    all_points = [p.vector for p in data]
    print(indices[0])
    print(','.join([str(i) for i in indices]))
    final_centroids = mykmeanssp.fit(k, iter, data, centroids, data[0].dimention, eps, centroid, datapoint)

    for centroid in final_centroids:
        print(','.join(f"{x:.4f}" for x in centroid.vector))
   # &k, &iter, &py_data, &py_clusters, &data_size, &dimention, &epsilon