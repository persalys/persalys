#!/bin/sh

# docker build docker/salome -t salome && docker run --rm --volume `pwd`:/io salome sh -c "/io/docker/salome/run_docker_build.sh `id -u` `id -g`"

set -x -e

uid=$1
gid=$2

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
sudo cp -r /tmp/persalys.AppDir/usr/lib/* /usr/local/lib # needed for yacs container
make tests
/home/devel/appli/salome -t
xvfb-run -s "-screen 0 1024x768x24" /home/devel/appli/salome shell -- ctest --output-on-failure --timeout 60 -j8 -E "Window|Wizard"

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
rm -rf /tmp/appli
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

# system libs
for libname in tbb fontconfig freetype sz gfortran quadmath pcre16 graphite2 swresample va va-drm va-x11 vdpau zvbi xvidcore webpmux wavpack vpx vorbisenc vorbis twolame ffi tasn1 nettle hogweed lz4 krb5 k5crypto krb5support xml2 cminpack event-2.0 harfbuzz re2 asound opus webp webpdemux xslt snappy minizip proxy aec double-conversion theoraenc theoradec speex shine openjp2 mp3lame gsm crystalhd ssh-gcrypt openmpt gme bluray chromaprint soxr numa ogg cairo
do
  cp -v /usr/lib/x86_64-linux-gnu/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/lib/x86_64-linux-gnu/libQt5*.so.[0-9] persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libboost* persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libhdf5_*.so.100 /usr/lib/x86_64-linux-gnu/libgmp.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libav*.so.57 /usr/lib/x86_64-linux-gnu/libavutil.so.55 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libicu*.so.57 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libgnutls.so.30 /usr/lib/x86_64-linux-gnu/libpng16.so.16 /usr/lib/x86_64-linux-gnu/libjpeg.so.62 /usr/lib/x86_64-linux-gnu/libprotobuf.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libx265.so.95 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libx264.so.148 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libplc4.so /usr/lib/x86_64-linux-gnu/libnss3.so /usr/lib/x86_64-linux-gnu/libsmime3.so /usr/lib/x86_64-linux-gnu/libnspr4.so /usr/lib/x86_64-linux-gnu/libplds4.so persalys.AppDir/usr/lib

cp -v /usr/lib/liblapack.so.3 /usr/lib/libblas.so.3 /usr/lib/libsrtp.so.0 persalys.AppDir/usr/lib
cp -v /lib/x86_64-linux-gnu/libbz2.so.1.0 /lib/x86_64-linux-gnu/libdbus-1.so.3 /lib/x86_64-linux-gnu/liblzma.so.5 /lib/x86_64-linux-gnu/libgpg-error.so.0 /lib/x86_64-linux-gnu/libexpat.so.1 /lib/x86_64-linux-gnu/libglib-2.0.so.0 persalys.AppDir/usr/lib
cp -v /usr/local/lib/lib*.so persalys.AppDir/usr/lib
cp -v /usr/local/lib/lib*.so.[0-9] persalys.AppDir/usr/lib
cp -v /usr/local/lib/libpython3.6m.so.1.0 persalys.AppDir/usr/lib
cp -v /home/devel/local/lib/lib*.so persalys.AppDir/usr/lib

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

LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib ldd persalys.AppDir/usr/bin/persalys

appimagetool -v persalys.AppDir persalys-`cat /io/VERSION`-`uname -p`.AppImage

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp persalys*.AppImage /io
  sudo chown ${uid}:${gid} /io/persalys*.AppImage
fi
