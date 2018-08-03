FROM centos:6
MAINTAINER jschueller

ENV MAKEFLAGS -j8
WORKDIR /usr/local/src

# utils
RUN yum install -y epel-release wget git patch unzip perl subversion xz

# devtoolset2
RUN wget -O /etc/yum.repos.d/devtools-2.repo http://people.centos.org/tru/devtools-2/devtools-2.repo
RUN yum -y update
RUN yum -y install devtoolset-2-gcc devtoolset-2-gcc-c++ devtoolset-2-gcc-gfortran devtoolset-2-binutils
ENV PATH /opt/rh/devtoolset-2/root/usr/bin:$PATH
ENV LD_LIBRARY_PATH /opt/rh/devtoolset-2/root/usr/lib64:/opt/rh/devtoolset-2/root/usr/lib

# muparser
RUN git clone https://github.com/beltoforion/muparser.git \
 && cd muparser && git checkout v2.2.5 && ./configure --disable-samples && make && make install

# swig
RUN yum -y install pcre-devel
RUN wget --no-check-certificate http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz && tar xzf swig-3.0.12.tar.gz \
 && cd swig-3.0.12 && ./configure --without-alllang && make && make install && swig -version

# cmake
RUN wget --no-check-certificate https://cmake.org/files/v3.9/cmake-3.9.6.tar.gz && tar xzf cmake-3.9.6.tar.gz \
 && cd cmake-3.9.6 && ./bootstrap --parallel=8 && make install

# nlopt
RUN git clone https://github.com/stevengj/nlopt.git && cd nlopt \
 && cmake -DINSTALL_LIB_DIR=/usr/local/lib -DBUILD_SHARED_LIBS=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_MATLAB=OFF . && make install

# tbb
RUN git clone https://github.com/wjakob/tbb.git && cd tbb && cmake -DTBB_BUILD_TESTS=OFF . && make && make install

# appimagetool
RUN wget https://github.com/AppImage/AppImageKit/releases/download/10/appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage \
 && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root \
 && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool -h

# glibc 2.14
RUN wget http://ftp.gnu.org/gnu/glibc/glibc-2.14.tar.gz && tar zxvf glibc-2.14.tar.gz \
 && cd glibc-2.14 && mkdir build && cd build && ../configure --prefix=/opt/glibc-2.14 && make > /dev/null && make install

# texlive: see https://github.com/leodido/dockerfiles/blob/master/texlive:latest/Dockerfile
RUN yum install -y perl fontconfig
# environment
ENV TL install-tl
RUN mkdir -p $TL
# texlive net batch installation
RUN wget -nv -O $TL.tar.gz http://mirror.ctan.org/systems/texlive/tlnet/install-tl-unx.tar.gz
RUN tar -xzf $TL.tar.gz -C $TL --strip-components=1
ADD full.profile $TL/
RUN cd $TL/ && LD_LIBRARY_PATH=/opt/glibc-2.14/lib ./install-tl --persistent-downloads  --profile full.profile
RUN ln -s /usr/local/texlive/2014/bin/x86_64-linux /opt/texbin
ENV PATH $PATH:/usr/local/texlive/2014/bin/x86_64-linux

# relink texlive executables
RUN wget http://springdale.math.ias.edu/data/puias/computational/6/x86_64/patchelf-0.8-2.sdl6.x86_64.rpm && rpm -Uvh patchelf*.rpm
RUN patchelf --set-rpath /opt/glibc-2.14/lib /opt/texbin/pdftex \
 && patchelf --set-rpath /opt/glibc-2.14/lib /opt/texbin/bibtex \
 && patchelf --set-rpath /opt/glibc-2.14/lib /opt/texbin/kpsewhich \
 && latex --version

# cleanup
RUN tlmgr install ucs anyfontsize preview
RUN rm $TL.tar.gz && rm -r $TL
# tex fonts installation
#RUN cp $(kpsewhich -var-value TEXMFSYSVAR)/fonts/conf/texlive-fontconfig.conf /etc/fonts/conf.d/09-texlive.conf
RUN fc-cache -fsv

# python
RUN yum -y install zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel gpg openssl-devel
RUN wget https://www.python.org/ftp/python/3.6.5/Python-3.6.5.tgz && tar xzf Python-3.6.5.tgz && cd Python-3.6.5 \
 && ./configure --enable-shared --enable-unicode=ucs4 LDFLAGS="-Wl,-rpath /usr/local/lib" && make && make install

# pip
RUN curl -sSLO https://bootstrap.pypa.io/get-pip.py && python3 get-pip.py \
 && pip install -U pip wheel setuptools && pip install scipy matplotlib ipython --upgrade

# sundials
RUN wget http://computation.llnl.gov/projects/sundials/download/sundials-2.7.0.tar.gz && tar xzf sundials-2.7.0.tar.gz \
 && cd sundials-2.7.0 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF \
    -DEXAMPLES_INSTALL=OFF -DLAPACK_ENABLE=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_TYPE=int32_t .. && make install

# fmilib
# wget http://www.jmodelica.org/downloads/FMIL/FMILibrary-2.0.3-src.zip && unzip FMILibrary-2.0.3-src.zip
RUN svn co https://svn.jmodelica.org/FMILibrary/tags/2.0.3/ FMILibrary-2.0.3 && cd FMILibrary-2.0.3 \
 && mkdir build && cd build && cmake -DFMILIB_INSTALL_PREFIX=/usr/local .. && make install

# pyfmi
RUN pip install cython && pip install lxml assimulo \
 && sed -i 's|print("Could not find GLIMDA")|pass|g' /usr/local/lib/python*/site-packages/assimulo/solvers/glimda.py \
 && python3 -c "import assimulo; import assimulo.solvers" \
 && wget https://pypi.io/packages/source/P/PyFMI/PyFMI-2.4.0.zip && unzip PyFMI-2.4.0.zip && cd PyFMI-2.4 \
 && wget https://github.com/conda-forge/pyfmi-feedstock/raw/master/recipe/get_temp_location-keyerror.patch \
 && patch -p1 -i get_temp_location-keyerror.patch && python3 setup.py install --fmil-home=/usr/local \
 && python3 -c "import pyfmi"

# sphinx
RUN yum -y install pandoc dvipng
RUN pip install sphinx numpydoc --upgrade

# boost
# https://svn.boost.org/trac10/ticket/11120
RUN ln -s /usr/local/include/python3.6m /usr/local/include/python3.6
RUN wget https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.bz2 && tar xjf boost_1_64_0.tar.bz2 && cd boost_1_64_0 \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python3 && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system -j8 install

# X11
RUN yum -y install libX11-devel libXt-devel libXext-devel chrpath libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel xorg-x11-server-Xvfb
RUN yum -y install mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel
RUN yum -y install freetype-devel libjpeg-turbo-devel libxcb-devel xcb-util xcb-util-devel fontconfig-devel xz-devel
RUN xvfb-run -h

# qt
RUN wget --no-check-certificate https://download.qt.io/archive/qt/5.11/5.11.1/single/qt-everywhere-src-5.11.1.tar.xz \
 && unxz qt-everywhere-src-5.11.1.tar.xz && tar xf qt-everywhere-src-5.11.1.tar && cd qt-everywhere-src-5.11.1 \
 && ./configure -skip qt3d -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtdeclarative -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview \
 -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent \
 -nomake tests -nomake examples && make && make install

# qwt
RUN wget --no-check-certificate http://downloads.sourceforge.net/qwt/qwt-6.1.3.tar.bz2 && tar xjf qwt-6.1.3.tar.bz2 \
 && cd qwt-6.1.3 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && qmake && make && make install

# paraview
RUN wget --no-check-certificate http://paraview.org/files/v5.5/ParaView-v5.5.2.tar.gz \
  && tar xzf ParaView-v5.5.2.tar.gz && cd ParaView-v5.5.2 \
  && wget https://gitlab.kitware.com/paraview/paraview/merge_requests/2474.patch && patch -p1 -i 2474.patch \
  && mkdir build && cd build && cmake \
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
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF\
  -DCMAKE_SKIP_RPATH=OFF \
  -DVTK_CUSTOM_LIBRARY_SUFFIX="" \
  -DVTK_INSTALL_LIBRARY_DIR="lib/paraview" \
  -DVTK_INSTALL_ARCHIVE_DIR="lib/paraview" \
  -DVTK_INSTALL_INCLUDE_DIR="include/paraview" \
  -DVTK_INSTALL_DATA_DIR="share/paraview" \
  -DVTK_INSTALL_DOC_DIR="share/doc/paraview" \
  -DVTK_INSTALL_PACKAGE_DIR="lib/cmake/paraview" \
  -DPARAVIEW_INSTALL_DEVELOPMENT_FILES=ON \
  -DPYTHON_EXECUTABLE=/usr/local/bin/python3 \
  -DCMAKE_INSTALL_PREFIX=/usr/local/ \
  .. && make install

# openturns
RUN yum -y install blas-devel lapack-devel libxml2-devel
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout 11c7f774521f9179985ae3515bc9f88cd2f5eb15 \
 && cmake -DLINK_PYTHON_LIBRARY=OFF -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 . && make install

# otmorris
RUN git clone https://github.com/openturns/otmorris.git && cd otmorris && git checkout v0.4 \
 && cmake -DUSE_SPHINX=OFF -DLINK_PYTHON_LIBRARY=OFF -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 . && make install

# otfmi
RUN git clone https://github.com/openturns/otfmi.git && cd otfmi && python3 setup.py install

# locale
RUN localedef -i en_US -f UTF-8 C.UTF-8
ENV LANG "C.UTF-8"

# devel user
RUN yum -y install sudo zip
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
