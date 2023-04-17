#!/bin/bash

set -ex

export PYTHONNOUSERSITE=True  # avoid conflicts with pip packages installed using --user

# NOTE: before the sdk was 10.9 which is not supported by arm64 (i.e. M1/M2 CPUs)
CMAKE_OSX_SYSROOT=/tmp/MacOSX-SDKs/MacOSX11.0.sdk
ARCH=$(uname -m) # arm64 | x86_64

# sdk
if [[ ! -d "/tmp/MacOSX-SDKs" ]]; then 
   pushd /tmp 
   rm -Rf MacOSX-SDKs 
   git clone https://github.com/phracker/MacOSX-SDKs.git 1>/dev/null
   popd 
fi

# install conda
if [[ ! -d "${HOME}/miniforge3" ]]; then 
   pushd ${HOME}
   curl -L -O https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-MacOSX-${ARCH}.sh
   bash Miniforge3-MacOSX-${ARCH}.sh -b # silent
   rm -f Miniforge3-MacOSX-${ARCH}.sh
   popd
fi

source ~/miniforge3/etc/profile.d/conda.sh || true # can be already activated
conda config --set always_yes yes --set anaconda_upload no

# activate conda
conda create --name my-env -c conda-forge python=${PYTHON_VERSION} numpy conda anaconda-client conda-build wheel swig cmake setuptools  
conda activate my-env

# compile 
mkdir -p build
pushd build
cmake  -GXcode  -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}  -DPython_EXECUTABLE=`which python` -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ../
cmake --build . --target ALL_BUILD --config Release --parallel 4
cmake --build . --target install	 --config Release 

# distrib
cd Release/pyplasm
rm -Rf $(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2") || true
python setup.py -q bdist_conda
CONDA_FILENAME=$(find ${CONDA_PREFIX} -iname "pyplasm*.tar.bz2" | head -n 1)
if [[ "${GIT_TAG}" != "" ]] ; then
   anaconda --verbose --show-traceback -t ${ANACONDA_TOKEN} upload ${CONDA_FILENAME}
fi

popd

rm -Rf build 1>/dev/null
conda deactivate
conda remove -n my-env --all 1>/dev/null

echo "DONE BuildVersion ${PYTHON_VERSION}"
