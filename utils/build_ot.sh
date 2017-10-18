#!/bin/sh

# installs openturns in ./ot-linux-install

set -e

git clone https://github.com/openturns/openturns.git
cd openturns
git checkout 1.10


cmake \
  -DCMAKE_INSTALL_PREFIX=$PWD/../ot-linux-install \
  -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang \
  -DPYTHON_INCLUDE_DIR=/usr/include/python2.7 -DPYTHON_LIBRARY=/usr/lib/x86_64-linux-gnu/libpython2.7.so \
  -DUSE_SPHINX=OFF \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  .
make install -j8
cd ..
