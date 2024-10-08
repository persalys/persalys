#!/bin/sh

# installs paraview in ./paraview/build/install

# sudo apt install lzma-dev libxt-dev libqt5x11extras5-dev qtxmlpatterns5-dev-tools libqt5svg5-dev qttools5-dev libglew-dev libfreetype6-dev
# sudo aptitude install libhdf5-dev libtheora-dev libnetcdf-dev libexpat-dev libjpeg8-dev libtiff-dev protobuf-c-compiler protobuf-compiler

set -e

export QT_SELECT=qt5

git clone --recursive --depth 1 -b v5.13.1 https://gitlab.kitware.com/paraview/paraview.git && cd paraview

cmake \
  -DPARAVIEW_BUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX=$PWD/build/install \
  -DPARAVIEW_USE_PYTHON=ON \
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
  -DCMAKE_INSTALL_RPATH=$PWD/build/install/lib/paraview \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF  \
  -DCMAKE_SKIP_RPATH=OFF \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF -DPARAVIEW_PLUGIN_DISABLE_XML_DOCUMENTATION=ON \
  -DPARAVIEW_USE_VTKM=OFF -DPARAVIEW_USE_FORTRAN=OFF \
  -DPARAVIEW_PLUGINS_DEFAULT=OFF -DPARAVIEW_PLUGIN_ENABLE_BagPlotViewsAndFilters=ON \
  -B build .
make install -C build -j16
