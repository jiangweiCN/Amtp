from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension


# setup(
#     ext_modules = cythonize("libjwumqa.py")
# )

extensions = [
    Extension("libjwumqa", ["libjwumqa.pyx"]),
]
setup(ext_modules=cythonize(extensions))

# setup(
#     name='libjwumqa_1.0.0',
#     ext_modules=cythonize("libjwumqa.py"),
# )
