import numpy as np
import pandas as pd
#do c api things

MAX_DISTANCE = 999999999999999

class centroid:
    def __init__(self, vector, points = set()):
        self.vector = vector
        self.points = points
        self.dimention = len(vector)
    
    def update_vector(self):
        if len(self.points) == 0:
            return
        for i in range(self.dimention):
            self.vector[i] = sum([p.vector[i] for p in self.points]) / len(self.points)
        return
    
    def add_point(self, p):
        if p.centroid:
            p.centroid.points.remove(p)
        p.centroid = self
        self.points.add(p)
        return

class datapoint:
    def __init__(self, vector, centroid = None):
        self.vector = vector
        self.centroid = centroid
        self.dimention = len(vector)
        self.curr_distance = -1

    def distance(self, other):
        return numpy.linalg.norm(self.vector - other.vector)
    

def parse_input(file1, file2):
    df1, df2 = pd.read_csv(file1, index_col=0), pd.read_csv(file2, index_col=0)
    data = df1.join(df2, how='inner')
    for line in data:
        

def choose_centroids(data, k):
    # TODO: change this to go over a pandas dataframe and not a list of objects maybe
    result = []
    result.append(centroid(numpy.random.choice(data, 1).vector))
    all_distances = 0
    for i in range(1, k):
        for p in data:
            to_last = p.distance(result[-1])
            if p.curr_distance < 0 or to_last < p.curr_distance:
                result[-1].add_point(p)
                all_distances += to_last - p.curr_distance
                p.curr_distance = to_last
        result.append(centroid(numpy.random.choice(data, 1, p=lambda x: x.curr_distance / all_distances).vector))
    return result