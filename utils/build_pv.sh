#!/bin/sh

# installs paraview in ./paraview/build/install

# sudo apt install qttools5-dev lzma-dev libxt-dev libqt5x11extras5-dev qtxmlpatterns5-dev-tools 
# sudo aptitude install libhdf5-dev libtheora-dev libnetcdf-dev libglew-dev libexpat-dev libfreetype6-dev libjpeg8-dev libqt5x11extras5-dev libtiff-dev qttools5-dev protobuf-c-compiler protobuf-compiler

set -e

export QT_SELECT=qt5


git clone https://gitlab.kitware.com/paraview/paraview.git
cd paraview
git checkout v5.6.0
git submodule init
git submodule update

mkdir -p build && cd build

cmake \
  -DCMAKE_RULE_MESSAGES=OFF \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DVTK_USE_64BIT_IDS=OFF \
  -DVTK_NO_PYTHON_THREADS=OFF \
  -DVTK_PYTHON_FULL_THREADSAFE=ON \
  -DPARAVIEW_ENABLE_PYTHON=ON \
  -DPYTHON_EXECUTABLE=/usr/bin/python3 \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF \
  -DBUILD_DOCUMENTATION=OFF \
  -DVTK_USE_SYSTEM_FREETYPE=ON \
  -DVTK_USE_SYSTEM_GL2PS=OFF \
  -DVTK_USE_SYSTEM_JPEG=ON \
  -DVTK_USE_SYSTEM_NETCDF=ON \
  -DVTK_USE_SYSTEM_GLEW=ON \
  -DVTK_USE_SYSTEM_PNG=ON \
  -DVTK_USE_SYSTEM_TIFF=ON \
  -DVTK_USE_SYSTEM_ZLIB=ON \
  -DVTK_USE_SYSTEM_LIBXML2=ON \
  -DVTK_USE_SYSTEM_EXPAT=ON \
  -DVTK_USE_SYSTEM_HDF5=ON \
  -DPARAVIEW_ENABLE_CATALYST:BOOL=ON \
  -DPARAVIEW_BUILD_QT_GUI:BOOL=ON \
  -DPARAVIEW_QT_VERSION=5 \
  -DVTK_BUILD_QT_DESIGNER_PLUGIN:BOOL=OFF \
  -DPARAVIEW_ENABLE_MATPLOTLIB:BOOL=ON \
  -DCMAKE_INSTALL_RPATH=$PWD/install/lib/paraview \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF  \
  -DCMAKE_SKIP_RPATH=OFF \
  -DVTK_CUSTOM_LIBRARY_SUFFIX="" \
  -DCMAKE_INSTALL_PREFIX=$PWD/install \
  -DVTK_INSTALL_LIBRARY_DIR="lib/paraview" \
  -DVTK_INSTALL_ARCHIVE_DIR="lib/paraview" \
  -DVTK_INSTALL_INCLUDE_DIR="include/paraview" \
  -DVTK_INSTALL_DATA_DIR="share/paraview" \
  -DVTK_INSTALL_DOC_DIR="share/doc/paraview" \
  -DVTK_INSTALL_PACKAGE_DIR="lib/cmake/paraview" \
  -DVTK_USE_SYSTEM_PROTOBUF=OFF \
  -DPARAVIEW_USE_VTKM=OFF \
  -DPARAVIEW_INSTALL_DEVELOPMENT_FILES:BOOL=ON \
  ..
make install -j8
cd ../..
