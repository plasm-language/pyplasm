#!/bin/bash

set -ex

PYTHON_VERSION=${PYTHON_VERSION:-3.8}
ANACONDA_TOKEN=${ANACONDA_TOKEN:-}

# skd
pushd /tmp 
rm -Rf MacOSX-SDKs 
git clone https://github.com/phracker/MacOSX-SDKs.git  --quiet
popd 
export CMAKE_OSX_SYSROOT=/tmp/MacOSX-SDKs/MacOSX10.9.sdk

# install conda
export PYTHONNOUSERSITE=True  # avoid conflicts with pip packages installed using --user
if [[ ! -d "~/miniforge3" ]]; then 
   pushd ~
   curl -L -O https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-MacOSX-x86_64.sh
   bash Miniforge3-MacOSX-x86_64.sh -b # silent
   rm -f Miniforge3-MacOSX-x86_64.sh
   popd
fi

# activate conda
source ~/miniforge3/etc/profile.d/conda.sh || true # can be already activated
conda config --set always_yes yes --set anaconda_upload no
conda create --name my-env -c conda-forge python=${PYTHON_VERSION} numpy conda anaconda-client conda-build wheel swig cmake setuptools  
conda activate my-env
PYTHON=`which python`

# compile 
BUILD_DIR=build_macos_conda
mkdir -p ${BUILD_DIR} 
cd ${BUILD_DIR}
cmake  -GXcode  -DCMAKE_OSX_SYSROOT=$CMAKE_OSX_SYSROOT  -DPython_EXECUTABLE=${PYTHON} ../
cmake --build . --target ALL_BUILD --config Release --parallel 4
cmake --build . --target install	 --config Release 

# distrib
pushd Release/pyplasm
rm -Rf $(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2") || true
$PYTHON setup.py -q bdist_conda
CONDA_FILENAME=$(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2" | head -n 1)
GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`
if [[ "${GIT_TAG}" != "" ]] ; then
   anaconda --verbose --show-traceback -t ${ANACONDA_TOKEN} upload ${CONDA_FILENAME}
fi
popd

echo "All done"