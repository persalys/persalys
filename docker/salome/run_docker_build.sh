#!/bin/sh

# docker build docker/salome -t persalys/salome && docker run --rm --volume `pwd`:/io persalys/salome sh -c "/io/docker/salome/run_docker_build.sh `id -u`:`id -g`"

set -x -e

UID_GID=$1

cd /tmp

cmake \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
  -DUSE_SALOME=ON \
  -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files \
  -DSalomeYACS_DIR=/home/devel/local/adm/cmake/ \
  -Dydefx_DIR=/home/devel/local/salome_adm/cmake_files/ \
  -DPy2cpp_DIR=/home/devel/local/lib/cmake/py2cpp \
  -DAdaoCppLayer_ROOT_DIR=/home/devel/local/ \
  -DCMAKE_INSTALL_PREFIX=/tmp/persalys.AppDir/usr \
  -B build /io
cd build
make install
sudo cp -r /tmp/persalys.AppDir/usr/lib/* /usr/local/lib # needed for yacs container
make tests
xvfb-run -s "-screen 0 1024x768x24" /home/devel/appli/salome shell -- ctest --output-on-failure --timeout 60 -j8 -E "Window|Wizard"

cd /tmp

rm -r persalys.AppDir/usr/include/persalys

cat > persalys.AppDir/AppRun <<\EOF
#!/bin/sh
HERE=$(dirname $(readlink -f "${0}"))
export PATH=${HERE}/usr/bin/:${HERE}/usr/salome/bin:${HERE}/usr/salome/bin/salome:${PATH}
export LD_LIBRARY_PATH=${HERE}/usr/lib/:${HERE}/usr/salome/lib/salome:${HERE}/usr/salome/lib:${HERE}/usr/salome/__RUN_SALOME__/lib/salome:${LD_LIBRARY_PATH}
export PYTHONHOME=${HERE}/usr/
export PYTHONPATH=${HERE}/usr/lib/python3.9/site-packages/salome:${HERE}/usr/lib/python3.9/site-packages:${HERE}/usr/bin/salome:${HERE}/usr/salome/lib/python3.9/site-packages:${HERE}/usr/salome/bin/salome:${HERE}/usr/salome/lib/salome:${HERE}/usr/salome/__RUN_SALOME__/lib/salome
export QT_PLUGIN_PATH=${HERE}/usr/lib/plugins

# assumes fonts are provided on host machine
export QT_QPA_FONTDIR=/usr/share/fonts/truetype

export OPENTURNS_CONFIG_PATH=${HERE}/etc/openturns

export KERNEL_ROOT_DIR=${HERE}/usr/salome
export YDEFX_ROOT_DIR=${HERE}/usr/salome
export USER_CATALOG_RESOURCES_FILE=/tmp/CatalogResources.xml

# salome virtual app
cp ${HERE}/salome_context.cfg ${HERE}/config_appli.xml ${HERE}/CatalogResources.xml /tmp
sed -i "s|/home/devel/local|${HERE}/usr/salome|g" /tmp/salome_context.cfg /tmp/config_appli.xml
sed -i "s|@USER@|${USER}|g" /tmp/CatalogResources.xml
rm -rf /tmp/appli
python3 ${HERE}/usr/salome/bin/salome/appli_gen.py --prefix=/tmp/appli --config=/tmp/config_appli.xml

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
for libname in fontconfig freetype sz gfortran quadmath graphite2 krb5 k5crypto krb5support xml2 cminpack harfbuzz aec double-conversion p11-kit
do
  cp -v /usr/lib/x86_64-linux-gnu/lib${libname}.so.[0-9] persalys.AppDir/usr/lib
done
cp -v /usr/lib/x86_64-linux-gnu/libQt5*.so.[0-9] persalys.AppDir/usr/lib
cp -v /usr/lib/libqwt-qt5.so.6 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libboost* persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libhdf5_*.so.103 /usr/lib/x86_64-linux-gnu/libgmp.so.10 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libgnutls.so.30 /usr/lib/x86_64-linux-gnu/libpng16.so.16 /usr/lib/x86_64-linux-gnu/libjpeg.so.62 persalys.AppDir/usr/lib
cp -v /usr/local/lib/lib*.so persalys.AppDir/usr/lib
cp -v /usr/local/lib/lib*.so.[0-9] persalys.AppDir/usr/lib
cp -v /usr/local/lib/libOT.so.0.27 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libpython3.9.so.1.0 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/liblapack.so.3 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libblas.so.3 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libicui18n.so.67 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libicuuc.so.67 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libpcre2-16.so.0 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libzstd.so.1 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libglib-2.0.so.0 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libmd4c.so.0 persalys.AppDir/usr/lib
cp -v /lib/x86_64-linux-gnu/libexpat.so.1 persalys.AppDir/usr/lib
cp -v /lib/x86_64-linux-gnu/liblzma.so.5 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libicudata.so.67 persalys.AppDir/usr/lib
cp -v /lib/x86_64-linux-gnu/libpcre.so.3 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libbrotlidec.so.1 persalys.AppDir/usr/lib
cp -v /usr/lib/x86_64-linux-gnu/libbrotlicommon.so.1 persalys.AppDir/usr/lib

# gdb
cp -v /usr/bin/gdb persalys.AppDir/usr/bin

# python
cp -v /usr/bin/python3* persalys.AppDir/usr/bin
cp -r /usr/lib/python3* persalys.AppDir/usr/lib
cp -r /usr/local/lib/python3* persalys.AppDir/usr/lib
cp -r /home/devel/local/lib/python3.9/site-packages/* persalys.AppDir/usr/lib/python3.9/site-packages

# salome
cp -v /usr/local/bin/omni* persalys.AppDir/usr/bin
cp -rv /home/devel/local persalys.AppDir/usr/salome
cp -v /tmp/salome_context.cfg /tmp/config_appli.xml /tmp/CatalogResources.xml persalys.AppDir/

cp -rv /usr/local/etc/ persalys.AppDir/etc

cp -rv /usr/lib/x86_64-linux-gnu/qt5/plugins persalys.AppDir/usr/lib
cp /usr/lib/x86_64-linux-gnu/libQt5XcbQpa.so.5 persalys.AppDir/usr/lib

LD_LIBRARY_PATH=$PWD/persalys.AppDir/usr/lib:$PWD/persalys.AppDir/usr/salome:$PWD/persalys.AppDir/usr/salome/lib ldd persalys.AppDir/usr/bin/persalys

APPIMAGE_FILE=persalys-salome-`cat /io/VERSION`-`uname -m`.AppImage
appimagetool -v persalys.AppDir ${APPIMAGE_FILE}

# copy to host with same permission
if test -n "${UID_GID}"
then
  sudo chown ${UID_GID} ${APPIMAGE_FILE}
  sudo cp -p ${APPIMAGE_FILE} /io
fi
