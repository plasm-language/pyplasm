#!/bin/bash

set -ex

# avoid conflicts with pip packages installed using --user
export PYTHONNOUSERSITE=True  

# install conda
pushd ~
curl -L -O https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh -b || true # it can already exists
rm -f Miniforge3-Linux-x86_64.sh
popd

# activate conda
source ~/miniforge3/etc/profile.d/conda.sh || true # can be already activated
conda config --set always_yes yes --set anaconda_upload no
conda create --name my-env -c conda-forge python=${PYTHON_VERSION} numpy conda anaconda-client conda-build wheel setuptools 
conda activate my-env

PYTHON=`which python`

# compile pyplas
mkdir -p build
cd build
cmake -DPython_EXECUTABLE=${PYTHON} ../
make -j
make install

# distrib
pushd Release/pyplasm 
rm -Rf $(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2") || true
$PYTHON setup.py -q bdist_conda
CONDA_FILENAME=$(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2" | head -n 1)
GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`
if [[ "${GIT_TAG}" != "" ]] ; then
  anaconda --verbose --show-traceback -t ${ANACONDA_TOKEN} upload ${CONDA_FILENAME} --no-progress 
fi
popd

echo "All done"




