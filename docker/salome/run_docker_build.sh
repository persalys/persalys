#!/bin/sh

set -x -e

export MAKEFLAGS="-j8"
cd /tmp

mkdir -p build && cd build
cmake \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DCMAKE_CXX_FLAGS="-Wall -Werror -D_GLIBCXX_ASSERTIONS" \
  -DCMAKE_INSTALL_PREFIX=$PWD/install \
  -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files \
  -DSalomeGUI_DIR=/home/devel/local/adm_local/cmake_files/ \
  -DSalomeYACS_DIR=/home/devel/local/adm/cmake/ \
  -Dydefx_DIR=/home/devel/local/salome_adm/cmake_files/ \
  -DPy2cpp_DIR=/home/devel/local/lib/cmake/py2cpp \
  -DAdaoCppLayer_ROOT_DIR=/home/devel/local/ \
  /io
make install
make tests
/home/devel/salome -t
xvfb-run --server-args="+render" /home/devel/salome shell -- ctest --output-on-failure --timeout 30 -j8
