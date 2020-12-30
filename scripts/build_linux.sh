#!/bin/bash
set -e  # stop or error
set -x  # very verbose
BUILD_DIR=${BUILD_DIR:-$(pwd)/build}
Python_EXECUTABLE=$(which python${PYTHON_VERSION})
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR} 

cmake -DPython_EXECUTABLE=${Python_EXECUTABLE} ../
cmake --build ./ --target all     --config Release --parallel 4
cmake --build ./ --target install --config Release

# test it
#cd Release/pyplasm
#export PYTHONPATH=../
#${Python_EXECUTABLE} -m pip install --upgrade pip numpy GL PyGlut PyOpenGL
#${Python_EXECUTABLE} -c "from pyplasm import *" || true  # segmentation fault 

