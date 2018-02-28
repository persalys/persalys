#!/bin/sh

set -x -e

uid=$1
gid=$2

cd /tmp

ARCH=x86_64
PYBASEVER=2.7
PYBASEVER_NODOT=${PYBASEVER:0:1}${PYBASEVER:2:2}
MINGW_PREFIX=/usr/${ARCH}-w64-mingw32/

export WINEARCH=win64
export WINEPATH=${MINGW_PREFIX}/bin
rm -rf ${HOME}/.wine*

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
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" ../otgui
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${MOD_PREFIX}/bin/*.dll
${ARCH}-w64-mingw32-strip --strip-unneeded ${MOD_PREFIX}/Lib/site-packages/otguibase/*.pyd
make tests
cp ${MINGW_PREFIX}/bin/*.dll ${MOD_PREFIX}/bin
cp -r ${MINGW_PREFIX}/lib/qt/plugins ${MOD_PREFIX}/lib
export WINEPATH=${MOD_PREFIX}/bin
${ARCH}-w64-mingw32-wine wineboot
xvfb-run ctest --output-on-failure --timeout 100 -j8 -E "FMI|WelcomeWindow"
VERSION=`cat ../otgui/VERSION`
cp /tmp/otgui/distro/windows/* .
makensis -DMODULE_PREFIX=${MOD_PREFIX} -DMODULE_VERSION=${VERSION} -DOPENTURNS_VERSION=1.10 -DPYBASEVER=${PYBASEVER} -DPYBASEVER_NODOT=${PYBASEVER_NODOT} -DARCH=${ARCH} installer.nsi

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp otgui*.exe /tmp/otgui
  sudo chown ${uid}:${gid} /tmp/otgui/otgui*.exe
fi
