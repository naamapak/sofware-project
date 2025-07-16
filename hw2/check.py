import mykmeanssp

# Call the fit function
result_centroids = mykmeanssp.fit([1, 2, 3], [[1, 2, 3], [4, 5, 6], [7, 8, 9], [10, 11, 12]], 100, 3, 4, 3, 0.11)

print(result_centroids)