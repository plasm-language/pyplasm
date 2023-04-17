#!/bin/bash

set -ex

GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`

# sdk
pushd /tmp 
rm -Rf MacOSX-SDKs 
git clone https://github.com/phracker/MacOSX-SDKs.git 1>/dev/null
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

# compile 
mkdir -p build
cd build
# -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" for M1/M2 Apple??
cmake  -GXcode  -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}  -DPython_EXECUTABLE=`which python`  ../
cmake --build . --target ALL_BUILD --config Release --parallel 4
cmake --build . --target install	 --config Release 

# distrib
cd Release/pyplasm
rm -Rf $(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2") || true
python setup.py -q bdist_conda
CONDA_FILENAME=$(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2" | head -n 1)
echo "CONDA_FILENAME=${CONDA_FILENAME}"
if [[ "${GIT_TAG}" != "" ]] ; then
   anaconda --verbose --show-traceback -t ${ANACONDA_TOKEN} upload ${CONDA_FILENAME}
fi

echo "All done"