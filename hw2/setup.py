from setuptools import Extension, setup

module = Extension("kmeansmodule",
                  sources=[
                    'kmeansmodule.c'
                  ])
setup(name='kmeansmodule',
     version='1.0',
     description='C extention for k-means implementation',
     ext_modules=[module])