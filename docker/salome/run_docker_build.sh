#!/bin/sh

# docker build docker/salome -t salome && docker run --rm --volume `pwd`:/io salome sh -c "/io/docker/salome/run_docker_build.sh `id -u` `id -g`"

set -x -e

uid=$1
gid=$2

export MAKEFLAGS="-j8"
cd /tmp

mkdir -p build && cd build
cmake \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DCMAKE_CXX_FLAGS="-Wall -Werror -D_GLIBCXX_ASSERTIONS" \
  -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files \
  -DSalomeGUI_DIR=/home/devel/local/adm_local/cmake_files/ \
  -DSalomeYACS_DIR=/home/devel/local/adm/cmake/ \
  -Dydefx_DIR=/home/devel/local/salome_adm/cmake_files/ \
  -DPy2cpp_DIR=/home/devel/local/lib/cmake/py2cpp \
  -DAdaoCppLayer_ROOT_DIR=/home/devel/local/ \
  -DCMAKE_INSTALL_PREFIX=/tmp/persalys.AppDir/usr \
  /io
make install
make tests
/home/devel/appli/salome -t
xvfb-run -s "-screen 0 1024x768x24" /home/devel/appli/salome shell -- ctest --output-on-failure --timeout 30 -j8 -E "Window|Wizard"


cd /tmp

rm -r persalys.AppDir/usr/include/persalys

cat > persalys.AppDir/AppRun <<\EOF
#!/bin/sh
HERE=$(dirname $(readlink -f "${0}"))
export PATH=${HERE}/usr/bin/:${HERE}/usr/salome/bin:${HERE}/usr/salome/bin/salome:${PATH}
export LD_LIBRARY_PATH=${HERE}/usr/lib/:${LD_LIBRARY_PATH}
export PYTHONHOME=${HERE}/usr/
export PYTHONPATH=${HERE}/usr/lib/python3.6/site-packages/salome:${HERE}/usr/bin/salome:${HERE}/usr/salome/lib/python3.6/site-packages
export QT_PLUGIN_PATH=${HERE}/usr/lib/plugins

# assumes fonts are provided on host machine
export QT_QPA_FONTDIR=/usr/share/fonts/truetype

export OPENTURNS_CONFIG_PATH=${HERE}/etc/openturns

export KERNEL_ROOT_DIR=${HERE}/usr/salome
export GUI_ROOT_DIR=${HERE}/usr/salome
export YDEFX_ROOT_DIR=${HERE}/usr/salome

# salome virtual app
cp ${HERE}/salome_context.cfg ${HERE}/config_appli.xml /tmp
sed -i "s|/home/devel/local|${HERE}/usr/salome|g" /tmp/salome_context.cfg /tmp/config_appli.xml
python3 ${HERE}/usr/salome/bin/salome/appli_gen.py --prefix=/tmp/appli --config=/tmp/config_appli.xml
/tmp/appli/salome -t

/tmp/appli/salome shell -- ${HERE}/usr/bin/persalys "$@"
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

libs=`ldd persalys.AppDir/usr/bin/persalys | cut -d " " -f 3`
for lib in ${libs}
do
  fn=`basename ${lib}`
  if ! test -f persalys.AppDir/usr/lib/${fn}
  then
    cp -v ${lib} persalys.AppDir/usr/lib
  fi
done
libs=`ldd /usr/lib/x86_64-linux-gnu/qt5/plugins/platforms/libqxcb.so | cut -d " " -f 3`
for lib in ${libs}
do
  fn=`basename ${lib}`
  if ! test -f persalys.AppDir/usr/lib/${fn}
  then
    cp -v ${lib} persalys.AppDir/usr/lib
  fi
done


# python
cp -v /usr/local/bin/python3* persalys.AppDir/usr/bin
cp -r /usr/local/lib/python3* persalys.AppDir/usr/lib
cp -r /home/devel/local/lib/python3.6/site-packages/* persalys.AppDir/usr/lib/python3.6/site-packages

# salome
cp -v /usr/local/bin/omni* persalys.AppDir/usr/bin
cp -rv /home/devel/local persalys.AppDir/usr/salome
cp -v /tmp/salome_context.cfg /tmp/config_appli.xml persalys.AppDir/

cp -rv /usr/local/etc/ persalys.AppDir/etc

cp -rv /usr/lib/x86_64-linux-gnu/qt5/plugins persalys.AppDir/usr/lib
cp /usr/lib/x86_64-linux-gnu/libQt5XcbQpa.so.5 persalys.AppDir/usr/lib

for libname in libpanelw.so.5 libncursesw.so.5 libgcc_s.so.1 libstdc++.so.6 libm.so.6 libdl.so.2 librt.so.1 libc.so.6 libnsl.so.1 libutil.so.1 libpthread.so.0 libresolv.so.2 libX11.so.6  libXext.so.6 libXrender.so.1 libICE.so.6 libSM.so.6 libGL.so.1 libgobject-2.0.so.0 libgthread-2.0.so.0 libglib-2.0.so.0
do
  rm -f persalys.AppDir/usr/lib/${libname}
done
rm persalys.AppDir/usr/lib/libdrm.so.2

LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib ldd persalys.AppDir/usr/bin/persalys

appimagetool -v persalys.AppDir persalys-`cat /io/VERSION`-`uname -p`.AppImage

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp persalys*.AppImage /io
  sudo chown ${uid}:${gid} /io/persalys*.AppImage
fi
