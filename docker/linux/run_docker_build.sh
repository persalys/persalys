#!/bin/sh

set -x -e

uid=$1
gid=$2

export MAKEFLAGS="-j8"
cd /io && ./utils/lint.sh
cd /tmp

mkdir -p build && cd build
cmake \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Werror -DPERSALYS_APPIMAGE" \
  -DCMAKE_SWIG_FLAGS="-Werror" -DSWIG_COMPILE_FLAGS="-Wno-unused-parameter -Wno-shadow" \
  -DPython_EXECUTABLE=/usr/local/bin/python3 \
  -DUSE_SPHINX=ON -DSPHINX_FLAGS="-W -T -j8" \
  -DCMAKE_INSTALL_PREFIX=/tmp/persalys.AppDir/usr -DCMAKE_INSTALL_LIBDIR=lib \
  -DOPENGL_opengl_LIBRARY=/usr/lib64/libGL.so \
  /io
make install
make tests
xvfb-run -s "-screen 0 1024x768x24" ctest --output-on-failure --timeout 30 -j8

cd /tmp/persalys.AppDir/usr/share/doc/persalys/; zip -r /tmp/persalys-doc.zip ./html/*; cd -

cd /tmp

rm -r persalys.AppDir/usr/include/persalys

cat > persalys.AppDir/AppRun <<\EOF
#!/bin/sh
HERE=$(dirname $(readlink -f "${0}"))
export PATH=${HERE}/usr/bin/:${PATH}
export LD_LIBRARY_PATH=${HERE}/usr/lib/:${HERE}/usr/lib/omc/
export PYTHONHOME=${HERE}/usr/
export PYTHONUSERBASE=${HOME}/.persalys_base
export PYTHONPATH=${PYTHONUSERBASE}/lib/python3.12/site-packages
mkdir -p ${PYTHONPATH}
export QT_PLUGIN_PATH=${HERE}/usr/lib/plugins
export PV_PLUGIN_PATH=${HERE}/usr/lib/paraview/plugins/BagPlotViewsAndFilters

# assumes fonts are provided on host machine
export QT_QPA_FONTDIR=/usr/share/fonts/truetype

export OPENTURNS_CONFIG_PATH=${HERE}/etc/openturns
export OPENMODELICAHOME=${HERE}/usr

${HERE}/usr/bin/persalys "$@"
EOF
chmod a+x persalys.AppDir/AppRun

cat > persalys.AppDir/persalys.desktop <<\EOF
[Desktop Entry]
Type=Application
Name=persalys
Comment=OpenTURNS gui
Exec=persalys
MimeType=application/x-persalys;
Icon=persalys
Terminal=false
Categories=Science;
EOF
cp -v /io/images/Ps-icon-32.png persalys.AppDir/persalys.png

# system libs
for libname in lapack blas quadmath xml2 gfortran ffi lzma harfbuzz mpfr mpc crypt freetype fontconfig
do
  cp -v /usr/lib64/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/lib64/libssl.so.10 /usr/lib64/libcrypto.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib64/libgmp.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib64/libtcl8.5.so /usr/lib64/libtk8.5.so persalys.AppDir/usr/lib
cp -v /usr/lib64/atlas/lib*.so.3 persalys.AppDir/usr/lib
cp -v /usr/lib64/libpng15.so.15 persalys.AppDir/usr/lib
cp -v /usr/lib64/libjpeg.so.62 persalys.AppDir/usr/lib
cp -v /usr/lib64/libicu*.so.50 persalys.AppDir/usr/lib
cp -v /usr/lib64/libxcb-*.so.[0-9] persalys.AppDir/usr/lib && rm persalys.AppDir/usr/lib/libxcb-dri3.so.0
cp -v /usr/lib64/libfreebl3.so persalys.AppDir/usr/lib
cp -v /lib64/libpcre2-16.so.0 persalys.AppDir/usr/lib

# python
sudo ln -sf /usr/local/bin/python3 /usr/local/bin/python
cp -v /usr/local/bin/python persalys.AppDir/usr/bin
cp -v /usr/local/bin/python3* persalys.AppDir/usr/bin
cp -r /usr/local/lib/python3* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libpython*.so.* persalys.AppDir/usr/lib

# ot libs
for libname in otmorris nlopt ceres cminpack bonmin ipopt coinmumps CoinUtils Cbc CbcSolver Cgl Osi OsiClp Clp ClpSolver
do
  cp -v /usr/local/lib/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/local/lib64/libpagmo.so.* persalys.AppDir/usr/lib
cp -v /usr/local/lib64/libtbb.so.* persalys.AppDir/usr/lib
cp -v /usr/local/lib64/libdlib* persalys.AppDir/usr/lib
cp -v /usr/local/lib64/libglog.so.1 persalys.AppDir/usr/lib
cp -v /usr/local/lib64/libprimesieve.so.* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libboost* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libmetis* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libGKlib* persalys.AppDir/usr/lib
cp -r /usr/local/lib/libhdf5* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libOT.so.0.* persalys.AppDir/usr/lib
cp -rv /usr/local/etc/ persalys.AppDir/etc

# qt libs
for libname in XcbQpa DBus Widgets Gui Test Core PrintSupport OpenGL Svg Concurrent Help Network X11Extras Sql WaylandClient
do
  cp -v /usr/local/lib/libQt5${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/local/lib/libqwt.so.6.* persalys.AppDir/usr/lib
cp -rv /usr/local/lib/qt/plugins persalys.AppDir/usr/lib

# paraview libs
cp -v /usr/local/lib/libvtk* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libpq* persalys.AppDir/usr/lib
mkdir -p persalys.AppDir/usr/lib/paraview/plugins
cp -rv /usr/local/lib/paraview*/plugins/BagPlotViewsAndFilters persalys.AppDir/usr/lib/paraview/plugins

# modelica
cp -rv /usr/local/lib64/omc persalys.AppDir/usr/lib
cp -rv /usr/local/include/omc persalys.AppDir/usr/include
cp -rv /usr/local/share/omc persalys.AppDir/usr/share
cp -v /usr/lib64/liblpsolve55.so /lib64/libuuid.so.1 /lib64/libexpat.so.1 persalys.AppDir/usr/lib
cp -v /usr/local/bin/omc persalys.AppDir/usr/bin
cp -v /usr/local/lib/libsundials* persalys.AppDir/usr/lib

# gdb
cp -v /usr/local/bin/gdb* persalys.AppDir/usr/bin
cp -v /usr/lib64/libtinfo.so.5 /usr/lib64/libncursesw.so.5 /usr/lib64/libpanelw.so.5 persalys.AppDir/usr/lib

LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib ldd persalys.AppDir/usr/lib/plugins/platforms/libqxcb.so
LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib ldd persalys.AppDir/usr/bin/persalys

appimagetool -v persalys.AppDir persalys-`cat /io/VERSION`-`uname -p`.AppImage

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp persalys*.AppImage persalys-doc.zip /io
  sudo chown ${uid}:${gid} /io/persalys*.AppImage /io/persalys-doc.zip
fi
