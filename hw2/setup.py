from setuptools import Extension, setup

module = Extension("mykmeanssp",
                  sources=[
                    'kmeansmodule.c'
                  ])
setup(name='mykmeanssp',
     version='1.0',
     description='C extention for k-means implementation',
     ext_modules=[module])