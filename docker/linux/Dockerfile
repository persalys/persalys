FROM centos:6
MAINTAINER jschueller

ENV MAKEFLAGS -j8
WORKDIR /usr/local/src

# utils
RUN yum -y install wget git patch zip unzip perl subversion xz gettext autoconf automake libtool pcre-devel perl fontconfig blas-devel lapack-devel atlas-devel libxml2-devel sudo
RUN yum -y install epel-release && yum -y install patchelf pandoc dvisvgm

# devtoolset-6
RUN yum -y install centos-release-scl && yum -y install devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-gcc-gfortran devtoolset-8-binutils
ENV PATH /opt/rh/devtoolset-8/root/usr/bin:$PATH
ENV LD_LIBRARY_PATH /opt/rh/devtoolset-8/root/usr/lib64:/opt/rh/devtoolset-8/root/usr/lib

# swig
RUN curl -fSsL http://prdownloads.sourceforge.net/swig/swig-4.0.2.tar.gz | tar xz && cd swig-4.0.2 \
 && ./configure --without-alllang && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && swig -version

# cmake
RUN curl -fSsL https://cmake.org/files/v3.17/cmake-3.17.3.tar.gz | tar xz && cd cmake-3.17.3 \
 && ./bootstrap --parallel=8 -- -DCMAKE_USE_OPENSSL=OFF && make install > /dev/null 2>&1

# cminpack
RUN curl -fsSL https://github.com/devernay/cminpack/archive/v1.3.6.tar.gz | tar xz && cd cminpack-1.3.6 \
 && cmake -DCMINPACK_LIB_INSTALL_DIR=lib -DBUILD_SHARED_LIBS=ON -DBUILD_EXAMPLES=OFF -DBLAS_LIBRARIES="/usr/lib64/atlas/libcblas.so.3.0;blas;m" . && make install

# nlopt
RUN curl -fSsL https://github.com/stevengj/nlopt/archive/v2.6.2.tar.gz | tar xz && cd nlopt-2.6.2 \
 && cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_MATLAB=OFF . && make install > /dev/null 2>&1

# eigen
RUN curl -fsSL http://bitbucket.org/eigen/eigen/get/3.3.7.tar.bz2 | tar xj && cd eigen-eigen-323c052e1731 \
 && mkdir build && cd build && cmake -DBUILD_EXAMPLES=OFF .. && make install

# spectra
RUN curl -fsSL https://github.com/yixuan/spectra/archive/v0.9.0.tar.gz | tar xz && cd spectra-0.9.0 && cmake . && make install

# glog
RUN curl -fsSL https://github.com/google/glog/archive/v0.4.0.tar.gz | tar xz && cd glog-0.4.0 \
 && cmake . && make install

# ceres
RUN curl -fsSL https://github.com/ceres-solver/ceres-solver/archive/1.14.0.tar.gz | tar xz && cd ceres-solver-1.14.0 \
&& cmake -DLIB_SUFFIX="" -DBUILD_SHARED_LIBS=ON . && make install

# tbb
RUN git clone https://github.com/wjakob/tbb.git && cd tbb && cmake -DTBB_BUILD_TESTS=OFF . && make install > /dev/null 2>&1
# https://bugreports.qt.io/browse/QTBUG-80990
RUN sed -i 's|#ifndef __TBB_profiling_H|#if !defined(__TBB_profiling_H) \&\& !defined(Q_MOC_RUN)|g' /usr/local/include/tbb/tbb_profiling.h

# appimagetool
RUN wget -q https://github.com/AppImage/AppImageKit/releases/download/12/appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage \
 && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root \
 && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool -h

# glibc 2.14
RUN curl -fSsL http://ftp.gnu.org/gnu/glibc/glibc-2.14.tar.gz | tar xz && cd glibc-2.14 && curl -L https://raw.githubusercontent.com/wheybags/glibc_version_header/master/patches/fix_obstack_compat.diff | patch -p1 && sed -i 's/3.4\* | 4.\[0-9\]\*/3.4* | 4.\[0-9\]\* | 8.\[0-9\]\*/g' configure \
 && mkdir build && cd build && ../configure --prefix=/opt/glibc-2.14 && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# texlive: see https://github.com/leodido/dockerfiles/blob/master/texlive:latest/Dockerfile
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
RUN yum -y install zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel gpg libffi-devel
RUN curl -fSsL https://www.openssl.org/source/openssl-1.0.2s.tar.gz | tar xz && cd openssl-1.0.2s \
 && ./config no-ssl2 no-shared -fPIC --prefix=/usr/local/ssl && make depend > /dev/null 2>&1 && make > /dev/null 2>&1 && make install_sw > /dev/null 2>&1
RUN curl -fSsL https://www.python.org/ftp/python/3.7.7/Python-3.7.7.tgz | tar xz && cd Python-3.7.7 \
 && ./configure --enable-shared CFLAGS="-I/usr/local/ssl/include" LDFLAGS="-Wl,-rpath /usr/local/lib -L/usr/local/ssl/lib" \
 && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && python3 -V && python3 -c "import ssl"

# pip
RUN curl -fsSLO https://bootstrap.pypa.io/get-pip.py && python3 get-pip.py && pip install -U pip wheel setuptools cython
RUN curl -fSsL https://github.com/numpy/numpy/archive/v1.17.5.tar.gz | tar xz && cd numpy-1.17.5 && python3 setup.py build ${MAKEFLAGS} && python3 setup.py install && cd - && python3 -c "import numpy"
RUN curl -fSsL https://github.com/scipy/scipy/archive/v1.1.0.tar.gz | tar xz && cd scipy-1.1.0 && python3 setup.py build ${MAKEFLAGS} && python3 setup.py install && cd - && python3 -c "import scipy.linalg"
RUN pip install lxml pandas matplotlib ipython --upgrade

# sundials
RUN curl -fSsL https://computation.llnl.gov/projects/sundials/download/sundials-4.1.0.tar.gz | tar xz && cd sundials-4.1.0 \
 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DEXAMPLES_INSTALL=OFF \
    -DLAPACK_ENABLE=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_TYPE=int32_t -DCMAKE_INSTALL_LIBDIR=lib .. && make install > /dev/null 2>&1

# fmilib
RUN curl -fSsL https://github.com/modelon-community/fmi-library/archive/2.2.tar.gz | tar xz && cd fmi-library-2.2 \
 && mkdir -p build && cd build && cmake -DFMILIB_INSTALL_PREFIX=/usr/local .. && make install > /dev/null 2>&1

# assimulo
RUN curl -fSsL https://github.com/modelon-community/Assimulo/archive/Assimulo-3.2.tar.gz | tar xz && cd Assimulo-Assimulo-3.2 && python3 setup.py install --extra-fortran-link-flags="-shared" --sundials-home=/usr/local --lapack-home=/usr/lib64 --blas-home=/usr/lib64 \
 && cd .. && python3 -c "import assimulo; import assimulo.solvers"

# pyfmi
RUN curl -fSsL https://github.com/modelon-community/PyFMI/archive/PyFMI-2.7.3.tar.gz | tar xz && cd PyFMI-PyFMI-2.7.3 \
 && python3 setup.py install --fmil-home=/usr/local && cd .. && python3 -c "import pyfmi"

# sphinx
RUN pip install sphinx numpydoc --upgrade

# boost
RUN curl -fSsL https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.tar.bz2 | tar xj && cd boost_1_73_0 \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python3 && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system ${MAKEFLAGS} install --with-locale --with-filesystem --with-regex > /dev/null 2>&1

# x11
RUN yum -y install libX11-devel libXt-devel libXext-devel chrpath libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel xorg-x11-server-Xvfb
RUN yum -y install mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel
RUN yum -y install freetype-devel libjpeg-turbo-devel fontconfig-devel xz-devel
RUN yum -y install xcb-util-renderutil-devel xcb-util-keysyms-devel xcb-util-image-devel xcb-util-wm-devel xorg-x11-util-macros bison
RUN curl -fSsL https://github.com/xkbcommon/libxkbcommon/archive/xkbcommon-0.8.4.tar.gz | tar xz && cd libxkbcommon-xkbcommon-0.8.4 && sed -i "/AX_GCC_BUILTIN/d" configure.ac && ./autogen.sh && ./configure --prefix=/usr --libdir=/usr/lib64 && make && make install && pkg-config --exists xkbcommon

# qt
RUN curl -fSsL http://mirrors.dotsrc.org/qtproject/archive/qt/5.15/5.15.0/single/qt-everywhere-src-5.15.0.tar.xz | tar xJ && cd qt-everywhere-src-5.15.0 && ./configure -xcb -no-mimetype-database -skip qt3d -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtdeclarative -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview \
 -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent \
 -nomake tests -nomake examples && make > /dev/null 2>&1 && make install > /dev/null 2>&1
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.0/submodules/qtx11extras-everywhere-src-5.15.0.tar.xz | tar xJ && cd qtx11extras-everywhere-src-5.15.0 && qmake src/x11extras && make > /dev/null 2>&1 && make install

# qwt
RUN curl -fSsL http://downloads.sourceforge.net/qwt/qwt-6.1.5.tar.bz2 | tar xj && cd qwt-6.1.5 \
 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# paraview
RUN curl -fSsL http://paraview.org/files/v5.8/ParaView-v5.8.0.tar.xz | tar xJ && cd ParaView-v5.8.0 \
  && mkdir build && cd build && cmake \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DVTK_USE_64BIT_IDS=OFF \
  -DVTK_NO_PYTHON_THREADS=OFF \
  -DVTK_PYTHON_FULL_THREADSAFE=ON \
  -DPARAVIEW_USE_PYTHON=ON \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF \
  -DPARAVIEW_USE_QT=ON \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=OFF\
  -DCMAKE_SKIP_RPATH=OFF \
  -DPARAVIEW_INSTALL_DEVELOPMENT_FILES=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local/ \
  .. && make install > /dev/null 2>&1

# openturns
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout v1.15 \
 && cmake  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1

# otmorris
RUN git clone https://github.com/openturns/otmorris.git && cd otmorris && git checkout v0.8 \
 && cmake -DUSE_SPHINX=OFF -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1

# otfmi
RUN pip install git+git://github.com/openturns/otfmi.git@v0.4 && python3 -c "import otfmi"

# locale
RUN localedef -i en_US -f UTF-8 C.UTF-8
ENV LANG "C.UTF-8"

# openmodelica
RUN yum -y install java-1.8.0-openjdk expat-devel lpsolve-devel libuuid-devel
RUN git clone --recursive https://github.com/OpenModelica/OpenModelica.git && cd OpenModelica && git checkout v1.14.1 \
 && git submodule sync && git submodule update --init --recursive \
 && cd OMCompiler && autoconf && CFLAGS="-fno-tree-vrp" ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# devel user
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
