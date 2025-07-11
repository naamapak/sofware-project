import numpy as np
import pandas as pd
import mykmeanssp
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
        return np.linalg.norm(self.vector - other.vector)
    

def parse_input(file1, file2):
    df1 = pd.read_csv(file1, index_col=0)
    df2 = pd.read_csv(file2, index_col=0)
    df = df1.join(df2, how='inner')
    datapoints = [datapoint(np.array(row)) for row in df.values]
    return datapoints, df

## did a whole new func (based on prev logic but better)
def choose_centroids(data, k):
    np.random.seed(1234)
    centroids = []

    # Step 1: choose first centroid randomly
    first = np.random.choice(data)
    centroids.append(first)

    for p in data:
        p.curr_distance = p.distance(first)

    for _ in range(1, k):
        distances = np.array([p.curr_distance ** 2 for p in data])
        probs = distances / distances.sum()
        chosen = np.random.choice(data, p=probs)
        centroids.append(chosen)

        # Update distances
        for p in data:
            dist = p.distance(chosen)
            if dist < p.curr_distance:
                p.curr_distance = dist

    return centroids

def parse_args():
    args = sys.argv
    if len(args) not in args[5,6]:
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
    centroids = choose_centroids(k)

    initial_centroids = [c.vector.tolist() for c in centroids]
    all_points = [p.vector.toalist() for p in data]

    final_centroids = mykmeanssp.fit(initial_centroids, all_points, k, iter, eps)
    