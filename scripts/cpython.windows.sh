#!/bin/bash

set -ex

BUILD_DIR=${BUILD_DIR:-build_windows}
PYTHON_VERSION=${PYTHON_VERSION:-3.8}
PYPI_USERNAME=${PYPI_USERNAME:-}
PYPI_TOKEN=${PYPI_TOKEN:-}

# swig
mkdir -p /tmp
pushd /tmp
__filename=https://cfhcable.dl.sourceforge.net/project/swig/swigwin/swigwin-4.0.2/swigwin-4.0.2.zip
curl -L --insecure  -O ${__filename} || \ curl -L --insecure  -O ${__filename} 
unzip -q swigwin-4.0.2.zip
SWIG_EXECUTABLE=$PWD/swigwin-4.0.2/swig.exe
popd

# cmake
mkdir -p /temp
pushd /tmp
__filename=https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1-windows-x86_64.zip
curl -L ${__filename} -O || \
curl -L ${__filename} -O
unzip -q cmake-3.22.1-windows-x86_64.zip 1>/dev/null
export PATH=$PATH:$PWD/cmake-3.22.1-windows-x86_64/bin
popd

PYTHON=$(which python)
$PYTHON -m pip install --upgrade pip	

# compile
mkdir -p ${BUILD_DIR} 
cd ${BUILD_DIR}
cmake -G "Visual Studio 16 2019" -A x64 -DPython_EXECUTABLE=${PYTHON} -DSWIG_EXECUTABLE=$SWIG_EXECUTABLE ../
cmake --build . --target ALL_BUILD --config Release --parallel 4
cmake --build . --target install   --config Release

# distrib
cd Release/pyplasm
rm -Rf ./dist
$PYTHON -m pip install setuptools wheel twine --upgrade 1>/dev/null || true
PYTHON_TAG=cp$(echo $PYTHON_VERSION | awk -F'.' '{print $1 $2}')
$PYTHON setup.py -q bdist_wheel --python-tag=${PYTHON_TAG} --plat-name=win_amd64
GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`
if [[ "${GIT_TAG}" != "" ]] ; then
	$PYTHON -m twine upload --username ${PYPI_USERNAME} --password ${PYPI_TOKEN} --skip-existing   "dist/*.whl" 
fi

echo "All done"