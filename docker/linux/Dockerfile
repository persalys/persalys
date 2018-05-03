FROM centos:6
MAINTAINER jschueller

ENV MAKEFLAGS -j8
WORKDIR /usr/local/src

# devtoolset2
RUN yum -y install wget epel-release
RUN yum install -y wget
RUN wget -O /etc/yum.repos.d/devtools-2.repo http://people.centos.org/tru/devtools-2/devtools-2.repo
RUN yum -y update
RUN yum -y install devtoolset-2-gcc devtoolset-2-gcc-c++ devtoolset-2-gcc-gfortran devtoolset-2-binutils
ENV PATH /opt/rh/devtoolset-2/root/usr/bin:$PATH
ENV LD_LIBRARY_PATH /opt/rh/devtoolset-2/root/usr/lib64:/opt/rh/devtoolset-2/root/usr/lib


# muparser
RUN yum install -y git
RUN git clone https://github.com/beltoforion/muparser.git
RUN cd muparser && git checkout v2.2.5 && ./configure --disable-samples && make && make install

# swig
RUN yum -y install pcre-devel
RUN wget --no-check-certificate http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
RUN tar xzf swig-3.0.12.tar.gz
RUN cd swig-3.0.12 && ./configure --without-alllang && make && make install
RUN swig -version

# cmake
RUN wget --no-check-certificate https://cmake.org/files/v3.9/cmake-3.9.6.tar.gz
RUN tar xzf cmake-3.9.6.tar.gz
RUN cd cmake-3.9.6 && ./bootstrap --parallel=8 && make install

# nlopt
RUN git clone https://github.com/stevengj/nlopt.git
RUN cd nlopt && cmake -DINSTALL_LIB_DIR=/usr/local/lib -DBUILD_SHARED_LIBS=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_MATLAB=OFF . && make install

# tbb
RUN git clone https://github.com/wjakob/tbb.git
RUN cd tbb && cmake -DTBB_BUILD_TESTS=OFF . && make && make install

# python
RUN yum -y install zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel gpg openssl-devel
RUN wget https://www.python.org/ftp/python/2.7.14/Python-2.7.14.tgz
RUN tar xzf Python-2.7.14.tgz
RUN cd Python-2.7.14 && ./configure --enable-shared --enable-unicode=ucs4 LDFLAGS="-Wl,-rpath /usr/local/lib" && make && make install

# pip
RUN curl -sSLO https://bootstrap.pypa.io/get-pip.py
RUN python get-pip.py
RUN pip install -U pip wheel setuptools
RUN yum -y install patch
# https://github.com/google/python-subprocess32/issues/12
RUN pip download subprocess32
RUN tar xzf subprocess32-3.2.7.tar.gz
RUN cd subprocess32-3.2.7 && wget https://github.com/google/python-subprocess32/commit/eae9e95f1167ee357b89d6e1823ddaeeaa373019.patch && patch -p1 -i eae9e95f1167ee357b89d6e1823ddaeeaa373019.patch && python ./setup.py install
RUN pip install scipy matplotlib ipython --upgrade

# sundials
RUN wget http://computation.llnl.gov/projects/sundials/download/sundials-2.7.0.tar.gz
RUN tar xzf sundials-2.7.0.tar.gz
RUN cd sundials-2.7.0 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF \
    -DEXAMPLES_INSTALL=OFF -DLAPACK_ENABLE=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_TYPE=int32_t .. && make install

# fmilib
RUN yum -y install unzip
# wget http://www.jmodelica.org/downloads/FMIL/FMILibrary-2.0.3-src.zip && unzip FMILibrary-2.0.3-src.zip
RUN yum -y install subversion && svn co https://svn.jmodelica.org/FMILibrary/tags/2.0.3/ FMILibrary-2.0.3 && cd FMILibrary-2.0.3 \
 && mkdir build && cd build && cmake -DFMILIB_INSTALL_PREFIX=/usr/local .. && make install

# pyfmi
RUN pip install cython && pip install lxml assimulo \
 && sed -i 's|print("Could not find GLIMDA")|pass|g' /usr/local/lib/python2.7/site-packages/assimulo/solvers/glimda.py \
 && python -c "import assimulo; import assimulo.solvers" \
 && wget https://pypi.io/packages/source/P/PyFMI/PyFMI-2.4.0.zip && unzip PyFMI-2.4.0.zip && cd PyFMI-2.4 \
 && wget https://github.com/conda-forge/pyfmi-feedstock/raw/master/recipe/get_temp_location-keyerror.patch \
 && patch -p1 -i get_temp_location-keyerror.patch && python setup.py install --fmil-home=/usr/local \
 && python -c "import pyfmi"

# boost
RUN wget https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.bz2 && tar xjf boost_1_64_0.tar.bz2 && cd boost_1_64_0 \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system -j8 install

# openturns
RUN yum -y install blas-devel lapack-devel libxml2-devel
RUN git clone https://github.com/openturns/openturns.git
RUN cd openturns && git checkout 1.10 && cmake -DLINK_PYTHON_LIBRARY=OFF -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" . && make install

# otmorris
RUN git clone https://github.com/openturns/otmorris.git
RUN cd otmorris && git checkout v0.3 && cmake -DUSE_SPHINX=OFF -DLINK_PYTHON_LIBRARY=OFF . && make install

# otfmi
RUN git clone https://github.com/openturns/otfmi.git
RUN cd otfmi && python setup.py install

# X11
RUN yum -y install libX11-devel libXt-devel libXext-devel chrpath libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel xorg-x11-server-Xvfb
RUN yum -y install mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel
RUN yum -y install freetype-devel libjpeg-turbo-devel libxcb-devel xcb-util xcb-util-devel fontconfig-devel xz-devel
RUN xvfb-run -h

# qt
RUN yum -y install xz
RUN wget --no-check-certificate https://download.qt.io/archive/qt/5.10/5.10.0/single/qt-everywhere-src-5.10.0.tar.xz \
 && unxz qt-everywhere-src-5.10.0.tar.xz && tar xf qt-everywhere-src-5.10.0.tar
# https://bugreports.qt.io/browse/QTBUG-64928
RUN cd qt-everywhere-src-5.10.0 && sed -i "450iif (connection->hasXRandr())" qtbase/src/plugins/platforms/xcb/qxcbscreen.cpp
RUN cd qt-everywhere-src-5.10.0 \
 && ./configure -skip qt3d -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtdeclarative -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview \
 -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent \
 -nomake tests -nomake examples && make && make install

# qwt
RUN wget --no-check-certificate http://downloads.sourceforge.net/qwt/qwt-6.1.3.tar.bz2
RUN tar xjf qwt-6.1.3.tar.bz2
RUN cd qwt-6.1.3 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri && qmake && make && make install

# paraview
#  -DVTK_USE_SYSTEM_OGGTHEORA=ON \
#  -DVTK_USE_SYSTEM_FREETYPE=ON \
#  -DVTK_USE_SYSTEM_GL2PS=OFF \
#  -DVTK_USE_SYSTEM_JPEG=ON \
#  -DVTK_USE_SYSTEM_NETCDF=ON \
#  -DVTK_USE_SYSTEM_GLEW=ON \
#  -DVTK_USE_SYSTEM_PNG=ON \
#  -DVTK_USE_SYSTEM_TIFF=ON \
#  -DVTK_USE_SYSTEM_ZLIB=ON \
#  -DVTK_USE_SYSTEM_LIBXML2=ON \
#  -DVTK_USE_SYSTEM_EXPAT=ON \
#  -DVTK_USE_SYSTEM_HDF5=ON \
#  -DVTK_USE_SYSTEM_PROTOBUF=ON \
RUN wget --no-check-certificate http://paraview.org/files/v5.4/ParaView-v5.4.1.tar.gz \
  && tar xzf ParaView-v5.4.1.tar.gz \
  && cd ParaView-v5.4.1 && mkdir build && cd build && cmake \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DVTK_USE_64BIT_IDS=OFF \
  -DVTK_NO_PYTHON_THREADS=OFF \
  -DVTK_PYTHON_FULL_THREADSAFE=ON \
  -DPARAVIEW_ENABLE_PYTHON=ON \
  -DBUILD_DOCUMENTATION=OFF \
  -DVTK_USE_OGGTHEORA_ENCODER=ON \
  -DPARAVIEW_ENABLE_CATALYST:BOOL=ON \
  -DPARAVIEW_BUILD_QT_GUI:BOOL=ON \
  -DPARAVIEW_QT_VERSION=5 \
  -DVTK_BUILD_QT_DESIGNER_PLUGIN:BOOL=OFF \
  -DPARAVIEW_ENABLE_MATPLOTLIB:BOOL=ON \
  -DPARAVIEW_BUILD_PLUGIN_AcceleratedAlgorithms:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_AdiosReader:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_AnalyzeNIfTIIO:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_ArrowGlyph:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_CDIReader:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_CatalystScriptGeneratorPlugin:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_LagrangianParticleTracker:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_EyeDomeLighting:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_GMVReader:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_GeodesicMeasurement:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_PacMan:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_StreamingParticles:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_Moments:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_NonOrthogonalSource:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_EyeDomeLighting:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_SLACTools:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_SurfaceLIC:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_SierraPlotTools:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_StreamingParticles:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_StreamLinesRepresentation:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_CatalystScriptGeneratorPlugin:BOOL=OFF \
  -DPARAVIEW_BUILD_PLUGIN_ThickenLayeredCells:BOOL=OFF \
  -DCMAKE_INSTALL_RPATH=$PWD/../../pv-linux-install/lib/paraview \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF\ 
  -DCMAKE_SKIP_RPATH=OFF \
  -DVTK_CUSTOM_LIBRARY_SUFFIX="" \
  -DVTK_INSTALL_LIBRARY_DIR="lib/paraview" \
  -DVTK_INSTALL_ARCHIVE_DIR="lib/paraview" \
  -DVTK_INSTALL_INCLUDE_DIR="include/paraview" \
  -DVTK_INSTALL_DATA_DIR="share/paraview" \
  -DVTK_INSTALL_DOC_DIR="share/doc/paraview" \
  -DVTK_INSTALL_PACKAGE_DIR="lib/cmake/paraview" \
  -DPARAVIEW_INSTALL_DEVELOPMENT_FILES:BOOL=ON \
  .. && make install

# appimagetool
RUN wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
RUN chmod a+rx appimagetool-x86_64.AppImage
RUN ./appimagetool-x86_64.AppImage --appimage-extract
RUN chmod -R a+rx squashfs-root
RUN ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool
RUN appimagetool -h

# texlive: see https://github.com/leodido/dockerfiles/blob/master/texlive:latest/Dockerfile
RUN yum install -y wget tar perl fontconfig
# envinronment
ENV TL install-tl
RUN mkdir -p $TL
# texlive net batch installation
RUN wget -nv -O $TL.tar.gz http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
RUN tar -xzf $TL.tar.gz -C $TL --strip-components=1
ADD full.profile $TL/
RUN cd $TL/ && ./install-tl --persistent-downloads  --profile full.profile
RUN ln -s /usr/local/texlive/2014/bin/x86_64-linux /opt/texbin
ENV PATH $PATH:/usr/local/texlive/2014/bin/x86_64-linux
# cleanup
RUN rm $TL.tar.gz && rm -r $TL
# tex fonts installation
#RUN cp $(kpsewhich -var-value TEXMFSYSVAR)/fonts/conf/texlive-fontconfig.conf /etc/fonts/conf.d/09-texlive.conf
RUN fc-cache -fsv

# sphinx
RUN yum -y install pandoc dvipng
RUN pip install sphinx numpydoc --upgrade

# devel user
RUN yum -y install sudo
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
