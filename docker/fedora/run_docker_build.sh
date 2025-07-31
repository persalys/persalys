#!/bin/sh

set -x -e
cd /tmp
cmake \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DCMAKE_INSTALL_PREFIX=$PWD/build/install \
  -DPERSALYS_QT_MAJOR_VERSION=6 \
  -B build /io
cd build
make install
make tests
xvfb-run -s "-screen 0 1024x768x24" ctest --output-on-failure --timeout 30 -j8
