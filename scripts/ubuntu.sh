#!/bin/bash

set -ex

BUILD_DIR=${BUILD_DIR:-build_docker}
PYTHON_VERSION=${PYTHON_VERSION:-3.8}
PYPI_USERNAME=${PYPI_USERNAME:-}
PYPI_TOKEN=${PYPI_TOKEN:-}
ANACONDA_TOKEN=${ANACONDA_TOKEN:-}

# needs to run using docker
DOCKER_IMAGE=${DOCKER_IMAGE:-}
if [[ "$DOCKER_IMAGE" != "" ]] ; then

  # run docker to compile portable pyplasm
  docker run --rm -v ${PWD}:/home/pyplasm -w /home/pyplasm \
    -e BUILD_DIR=build_docker \
    -e PYTHON_VERSION=${PYTHON_VERSION} \
    -e PYPI_USERNAME=${PYPI_USERNAME} -e PYPI_TOKEN=${PYPI_TOKEN} \
    -e ANACONDA_TOKEN=${ANACONDA_TOKEN} \
    -e INSIDE_DOCKER=1 \
    ${DOCKER_IMAGE} bash scripts/ubuntu.sh

  echo "All done ubuntu $PYTHON_VERSION} "
  exit 0

fi

# cpython 

PYTHON=`which python${PYTHON_VERSION}`

# this is for linux/docker (is this needed?)
yum install -y libffi-devel

# make sure pip is updated
${PYTHON} -m pip install --upgrade pip || true

# compile
mkdir -p ${BUILD_DIR} 
cd ${BUILD_DIR}
cmake -DPython_EXECUTABLE=${PYTHON} ../
make -j
make install

# distrib
pushd Release/pyplasm
rm -Rf ./dist
$PYTHON -m pip install --upgrade pip || true 
$PYTHON -m pip install setuptools wheel cryptography==3.4.0 twine || true
PYTHON_TAG=cp$(echo $PYTHON_VERSION | awk -F'.' '{print $1 $2}')
$PYTHON setup.py -q bdist_wheel --python-tag=$PYTHON_TAG --plat-name=manylinux2010_x86_64
GIT_TAG=`git describe --tags --exact-match 2>/dev/null || true`
if [[ "${GIT_TAG}" != "" ]] ; then
  $PYTHON -m twine upload --username ${PYPI_USERNAME} --password ${PYPI_TOKEN} --skip-existing   "dist/*.whl" 
fi
popd

# conda 

export PYTHONNOUSERSITE=True  # avoid conflicts with pip packages installed using --user
pushd ~
curl -L -O https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh -b || true # maybe it's already installed?
rm -f Miniforge3-Linux-x86_64.sh
popd
source ~/miniforge3/etc/profile.d/conda.sh || true # can be already activated
conda config --set always_yes yes --set anaconda_upload no
conda create --name my-env -c conda-forge python=${PYTHON_VERSION} numpy conda anaconda-client conda-build wheel pyopengl freeglut 
conda activate my-env
PYTHON=`which python`

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



