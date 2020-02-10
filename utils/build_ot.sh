#!/bin/sh

# installs openturns in ./openturns/build/install

set -e

git clone https://github.com/openturns/openturns.git
cd openturns
git checkout 1.14

cmake \
  -DCMAKE_INSTALL_PREFIX=$PWD/install \
  -DPYTHON_EXECUTABLE=/usr/bin/python3 \
  -DUSE_SPHINX=OFF \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j16" \
  .
make install -j16
cd ..
