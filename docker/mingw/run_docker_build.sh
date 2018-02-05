#!/bin/sh

set -x -e

cd /tmp

ARCH=i686
PYBASEVER=2.7
PYBASEVER_NODOT=${PYBASEVER:0:1}${PYBASEVER:2:2}

export WINEARCH=win32
export WINEPATH=${MINGW_PREFIX}/bin
rm -rf ${HOME}/.wine*

MINGW_PREFIX=/usr/${ARCH}-w64-mingw32/
export PYTHONHOME=${MINGW_PREFIX}
export PYTHONPATH=${MINGW_PREFIX}/lib/python${PYBASEVER_NODOT}
export MAKEFLAGS="-j8"

mkdir -p build && cd build
MOD_PREFIX=$PWD/install
CXXFLAGS="-D_hypot=hypot" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF -DUSE_PARAVIEW=OFF \
  -DCMAKE_INSTALL_PREFIX=${MOD_PREFIX} \
  -DPYTHON_INCLUDE_DIR=/usr/${ARCH}-w64-mingw32/include/python${PYBASEVER_NODOT} \
  -DPYTHON_LIBRARY=/usr/${ARCH}-w64-mingw32/lib/libpython${PYBASEVER_NODOT}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/${ARCH}-w64-mingw32/bin/python${PYBASEVER_NODOT}.exe \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" ../otgui
make install
make tests
cp ${MINGW_PREFIX}/bin/*.dll ${MOD_PREFIX}/bin
cp ${MINGW_PREFIX}/lib/qt/plugins/platforms/qwindows.dll ${MOD_PREFIX}/bin
export WINEPATH=${MOD_PREFIX}/bin
xvfb-run ctest --output-on-failure --timeout 100 -j8 -E "FMI|WelcomeWindow"

VERSION=`cat ../otgui/VERSION`
cp /tmp/otgui/distro/windows/* .
makensis -DMODULE_PREFIX=${MOD_PREFIX} -DMODULE_VERSION=${VERSION} -DOPENTURNS_VERSION=1.10 -DPYBASEVER=${PYBASEVER} -DPYBASEVER_NODOT=${PYBASEVER_NODOT} -DARCH=${ARCH} installer.nsi
