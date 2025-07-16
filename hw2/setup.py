from setuptools import setup, Extension

# Define the extension module
kmeans_module = Extension('mykmeanssp',
                            sources=['kmeansmodule.c', 'kmeans.c'])

# Run the setup
setup(name='mykmeanssp',
      version='1.0',
      description='Python interface for the C implementation of k-means algorithm',
      ext_modules=[kmeans_module])