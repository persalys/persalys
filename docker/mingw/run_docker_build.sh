#!/bin/sh

set -x -e

uid=$1
gid=$2

cd /tmp

mkdir -p build && cd build
MOD_PREFIX=$PWD/install
CXXFLAGS="-Wall -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF -DUSE_SALOME=OFF \
  -DCMAKE_INSTALL_PREFIX=${MOD_PREFIX} \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" ../otgui
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${MOD_PREFIX}/bin/*.dll
${ARCH}-w64-mingw32-strip --strip-unneeded ${MOD_PREFIX}/Lib/site-packages/otguibase/*.pyd
make tests
cp ${MINGW_PREFIX}/bin/*.dll ${MOD_PREFIX}/bin
cp -r ${MINGW_PREFIX}/lib/qt/plugins/platforms ${MOD_PREFIX}/lib
cp -r ${MINGW_PREFIX}/lib/qt/plugins/imageformats ${MOD_PREFIX}/lib
export WINEPATH="${MINGW_PREFIX}/bin;${MOD_PREFIX}/bin"
xvfb-run ctest --output-on-failure --timeout 100 -j8 -E FMI
VERSION=`cat ../otgui/VERSION`
cp /tmp/otgui/distro/windows/* .
unzip otgui-doc.zip -d .
makensis -DMODULE_PREFIX=${MOD_PREFIX} -DMODULE_VERSION=${VERSION} -DOPENTURNS_VERSION=1.12 -DPYBASEVER=${PYMAJMIN:0:1}.${PYMAJMIN:1:1} -DPYBASEVER_NODOT=${PYMAJMIN} -DARCH=${ARCH} installer.nsi

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp otgui*.exe /tmp/otgui
  sudo chown ${uid}:${gid} /tmp/otgui/otgui*.exe
fi
