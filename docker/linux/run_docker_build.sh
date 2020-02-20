#!/bin/sh

set -x -e

uid=$1
gid=$2

export MAKEFLAGS="-j8"
cd /tmp

mkdir -p build && cd build
cmake \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" \
  -DSWIG_COMPILE_FLAGS="-Wno-error=unused-parameter" \
  -DPYTHON_EXECUTABLE=/usr/local/bin/python3 \
  -DSPHINX_FLAGS="-W -T -j8" \
  -DCMAKE_INSTALL_PREFIX=/tmp/persalys.AppDir/usr \
  /io
make install
make tests
xvfb-run -s "-screen 0 1024x768x24" ctest --output-on-failure --timeout 30 -j8

cd /tmp/persalys.AppDir/usr/share/persalys/doc/; zip -r /tmp/persalys-doc.zip ./html/*; cd -

cd /tmp

rm -r persalys.AppDir/usr/include/persalys

cat > persalys.AppDir/AppRun <<\EOF
#!/bin/sh
HERE=$(dirname $(readlink -f "${0}"))
export PATH=${HERE}/usr/bin/:${PATH}
export LD_LIBRARY_PATH=${HERE}/usr/lib/:${HERE}/usr/lib/x86_64-linux-gnu/omc/:${LD_LIBRARY_PATH}
export PYTHONHOME=${HERE}/usr/
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
for libname in lapack blas xml2 png12 gfortran ffi
do
  cp -v /usr/lib64/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/lib64/libssl.so.10 /usr/lib64/libcrypto.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib64/libtcl8.5.so /usr/lib64/libtk8.5.so persalys.AppDir/usr/lib

# python
cp -v /usr/local/bin/python3* persalys.AppDir/usr/bin
cp -r /usr/local/lib/python3* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libpython*.so.* persalys.AppDir/usr/lib

# ot libs
for libname in otmorris OT muparser nlopt ceres cminpack
do
  cp -v /usr/local/lib/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/local/lib/libtbb.so persalys.AppDir/usr/lib
cp -v /usr/local/lib/libboost* persalys.AppDir/usr/lib
cp -rv /usr/local/etc/ persalys.AppDir/etc

# qt libs
for libname in Qt5XcbQpa Qt5DBus Qt5Widgets Qt5Gui Qt5Test Qt5Core qwt Qt5PrintSupport Qt5OpenGL Qt5Svg Qt5Concurrent Qt5Help Qt5Network Qt5X11Extras Qt5Sql
do
  cp -v /usr/local/lib/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -rv /usr/local/lib/qt/plugins persalys.AppDir/usr/lib

# paraview libs
cp -v /usr/local/lib/libvtk* persalys.AppDir/usr/lib
cp -v /usr/local/lib/libpq* persalys.AppDir/usr/lib
mkdir -p persalys.AppDir/usr/lib/paraview/plugins
cp -rv /usr/local/lib/paraview*/plugins/BagPlotViewsAndFilters persalys.AppDir/usr/lib/paraview/plugins

# modelica
cp -rv /usr/local/lib/x86_64-linux-gnu persalys.AppDir/usr/lib
cp -rv /usr/local/lib/omc persalys.AppDir/usr/lib
cp -rv /usr/local/include/omc persalys.AppDir/usr/include
cp -rv /usr/local/share/omc persalys.AppDir/usr/share
cp -v /usr/lib64/liblpsolve55.so /lib64/libuuid.so.1 /lib64/libexpat.so.1 persalys.AppDir/usr/lib
cp -v /usr/local/bin/omc persalys.AppDir/usr/bin
cp -v /usr/local/lib/libsundials* persalys.AppDir/usr/lib

LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib ldd persalys.AppDir/usr/bin/persalys

appimagetool -v persalys.AppDir persalys-`cat /io/VERSION`-`uname -p`.AppImage

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp persalys*.AppImage persalys-doc.zip /io
  sudo chown ${uid}:${gid} /io/persalys*.AppImage /io/persalys-doc.zip
fi
