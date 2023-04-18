

#!/bin/bash

set -ex

PYTHON=`which python${PYTHON_VERSION}`

# this is for linux/docker
yum install -y libffi-devel

# make sure pip is updated
${PYTHON} -m pip install --upgrade pip || true

# compile
mkdir -p build
cd build
cmake -DPython_EXECUTABLE=${PYTHON} ../
make -j
make install

# distrib
cd Release/pyplasm
rm -Rf ./dist
PYTHON_TAG=cp$(echo $PYTHON_VERSION | awk -F'.' '{print $1 $2}')
${PYTHON} -m pip install setuptools wheel cryptography==3.4.0 twine || true
${PYTHON} setup.py -q bdist_wheel --python-tag=${PYTHON_TAG} --plat-name=manylinux2010_x86_64
if [[ "${GIT_TAG}" != "" ]] ; then
  ${PYTHON} -m twine upload --username ${PYPI_USERNAME} --password ${PYPI_TOKEN} --skip-existing   "dist/*.whl" 
fi

echo "All done"