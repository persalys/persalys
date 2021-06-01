#!/bin/sh

# installs paraview in ./paraview/build/install

# sudo apt install qttools5-dev lzma-dev libxt-dev libqt5x11extras5-dev qtxmlpatterns5-dev-tools libqt5svg5-dev
# sudo aptitude install libhdf5-dev libtheora-dev libnetcdf-dev libglew-dev libexpat-dev libfreetype6-dev libjpeg8-dev libqt5x11extras5-dev libtiff-dev qttools5-dev protobuf-c-compiler protobuf-compiler

set -e

export QT_SELECT=qt5


# git clone https://gitlab.kitware.com/paraview/paraview.git && cd paraview && git checkout v5.9.1 && git submodule init && git submodule update
curl -fSsL http://paraview.org/files/v5.9/ParaView-v5.9.1.tar.gz | tar xz && cd ParaView-v5.9.1

mkdir -p build && cd build

cmake \
  -DPARAVIEW_BUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DPARAVIEW_USE_PYTHON=ON \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF \
  -DVTK_MODULE_USE_EXTERNAL_VTK_freetype=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_jpeg=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_netcdf=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_glew=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_png=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_tiff=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_zlib=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_libxml2=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_expat=ON \
  -DVTK_MODULE_USE_EXTERNAL_VTK_hdf5=ON \
  -DPARAVIEW_USE_QT:BOOL=ON \
  -DCMAKE_INSTALL_RPATH=$PWD/install/lib/paraview \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF  \
  -DCMAKE_SKIP_RPATH=OFF \
  -DCMAKE_INSTALL_PREFIX=$PWD/install \
  -DPARAVIEW_USE_VTKM=OFF \
  -DPARAVIEW_INSTALL_DEVELOPMENT_FILES:BOOL=ON \
  ..
make install -j16
cd ../..
