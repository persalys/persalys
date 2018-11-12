#!/bin/sh

set -x -e

uid=$1
gid=$2

export MAKEFLAGS="-j8"
cd /tmp

mkdir -p build && cd build
cmake -DUSE_COTIRE=ON \
-DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
-DCMAKE_CXX_FLAGS="-Wall -Werror -D_GLIBCXX_ASSERTIONS" \
-DPYTHON_EXECUTABLE=/usr/local/bin/python3 \
../otgui
make
sudo make install
make tests
xvfb-run ctest --output-on-failure --timeout 100 -j8

cd /usr/local/share/otgui/doc/ && zip -r /tmp/build/otgui-doc.zip ./html/* && cd -

mkdir -p otgui.AppDir/usr/{bin,lib,share}

cat > otgui.AppDir/AppRun <<\EOF
#!/bin/sh
HERE=$(dirname $(readlink -f "${0}"))
export PATH=${HERE}/usr/bin/:${PATH}
export LD_LIBRARY_PATH=${HERE}/usr/lib/:${LD_LIBRARY_PATH}
export PYTHONHOME=${HERE}/usr/
export QT_PLUGIN_PATH=${HERE}/usr/lib/plugins

# assumes fonts are provided on host machine
export QT_QPA_FONTDIR=/usr/share/fonts/truetype

export OPENTURNS_CONFIG_PATH=${HERE}/etc/openturns
${HERE}/usr/bin/otgui
EOF
chmod a+x otgui.AppDir/AppRun

cat > otgui.AppDir/otgui.desktop <<\EOF
[Desktop Entry]
Type=Application
Name=otgui
Comment=OpenTURNS gui
Exec=otgui
MimeType=application/x-otgui;
Icon=otgui
Terminal=false
EOF

cp -v /usr/local/bin/otgui otgui.AppDir/usr/bin
cp -rv /usr/local/lib/qt/plugins otgui.AppDir/usr/lib
cp -rv /usr/local/etc/ otgui.AppDir/etc
cp -rv /usr/local/share/otgui otgui.AppDir/usr/share

cp -v /usr/local/bin/python3.7 otgui.AppDir/usr/bin
cp -r /usr/local/lib/python3.7 otgui.AppDir/usr/lib

cp -v ../otgui/images/OT_icon32x32.png otgui.AppDir/otgui.png

for libname in otguiview otguiplotparaview otguipyconsole otguipyinterp otguiplot otguimodel otguibase otguiutils otmorris OT muparser nlopt
do
  cp -v /usr/local/lib/lib${libname}.so.[0-9] otgui.AppDir/usr/lib
done

# qt libs
for libname in Qt5XcbQpa Qt5DBus Qt5Widgets Qt5Gui Qt5Test Qt5Core qwt Qt5PrintSupport Qt5OpenGL Qt5Svg Qt5Concurrent Qt5Help Qt5Network Qt5X11Extras Qt5Sql
do
  cp -v /usr/local/lib/lib${libname}.so.[0-9] otgui.AppDir/usr/lib
done

cp -v /usr/local/lib/libpython*.so.* otgui.AppDir/usr/lib
cp -v /usr/local/lib/libtbb.so otgui.AppDir/usr/lib

# boost libs
cp -v /usr/local/lib/libboost_regex.so.1.*.0 otgui.AppDir/usr/lib
cp -v /usr/local/lib/libboost_filesystem.so.1.*.0 otgui.AppDir/usr/lib
cp -v /usr/local/lib/libboost_system.so.1.*.0 otgui.AppDir/usr/lib

# paraview libs
cp -v /usr/local/lib/paraview/libvtk* otgui.AppDir/usr/lib
cp /usr/local/lib/paraview/libQtTesting.so otgui.AppDir/usr/lib
cp /usr/local/lib/paraview/libprotobuf*.so otgui.AppDir/usr/lib


# system libs
for libname in lapack blas xml2 png12 gfortran
do
  cp -v /usr/lib64/lib${libname}.so.[0-9] otgui.AppDir/usr/lib
done
cp -v /usr/lib64/libssl.so.10 /usr/lib64/libcrypto.so.10 otgui.AppDir/usr/lib
cp -v /usr/lib64/libtcl8.5.so /usr/lib64/libtk8.5.so otgui.AppDir/usr/lib

LD_LIBRARY_PATH=$PWD/otgui.AppDir/usr/lib ldd otgui.AppDir/usr/bin/otgui

appimagetool -v otgui.AppDir

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp otgui*.AppImage otgui-doc.zip /tmp/otgui
  sudo chown ${uid}:${gid} /tmp/otgui/otgui*.AppImage /tmp/otgui/otgui-doc.zip
fi
