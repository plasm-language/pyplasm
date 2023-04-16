#!/bin/bash

set -ex

PYTHON_VERSION=${PYTHON_VERSION:-3.8}
ANACONDA_TOKEN=${ANACONDA_TOKEN:-}

GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`

# install conda
export PYTHONNOUSERSITE=True  # avoid conflicts with pip packages installed using --user
choco install --accept-license --yes --force miniconda3 
CONDA_HOME=/c/tools/miniconda3
echo "source ${CONDA_HOME}/etc/profile.d/conda.sh" >> ~/.bashrc
source ~/.bashrc
conda config  --set always_yes yes --set changeps1 no --set anaconda_upload no 
conda create --name my-env -c conda-forge python=${PYTHON_VERSION} numpy cmake swig anaconda-client wheel conda conda-build
conda activate my-env
PYTHON=`which python`

# compil
BUILD_DIR=build_windows_conda
mkdir -p ${BUILD_DIR} 
cd ${BUILD_DIR}
cmake -G "Visual Studio 16 2019" -A x64 \
    -DSWIG_EXECUTABLE=$(which swig) \
    -DPython_EXECUTABLE=${PYTHON} \
    ../ 
cmake --build . --target ALL_BUILD --config Release --parallel 8
cmake --build . --target install   --config Release

# for for `bdist_conda` problem
find ${CONDA_PREFIX} 
pushd ${CONDA_PREFIX}/Lib
cp -n distutils/command/bdist_conda.py site-packages/setuptools/_distutils/command/bdist_conda.py || true # just le'ts hope it will work anyway
popd

# distrib
pushd Release/pyplasm
rm -Rf $(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2") || true
$PYTHON setup.py -q bdist_conda 1>/dev/null
CONDA_FILENAME=$(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2" | head -n 1)
if [[ "${GIT_TAG}" != "" ]] ; then
    anaconda --verbose --show-traceback -t ${ANACONDA_TOKEN} upload ${CONDA_FILENAME}
fi
popd

echo "All done windows conda $PYTHON_VERSION}"
