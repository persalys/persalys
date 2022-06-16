FROM centos:7
MAINTAINER jschueller

# utils
RUN yum -y install epel-release centos-release-scl \
 && yum -y install wget git patch zip unzip bzip2 bsdtar perl subversion gettext autoconf automake libtool sudo bison flex make which file \
  pcre-devel blas-devel lapack-devel atlas-devel libxml2-devel libmpc-devel mpfr-devel \
  zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel gpg libffi-devel openssl-devel \
  libX11-devel libXt-devel libXext-devel libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel \
  xorg-x11-server-Xvfb mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel mesa-dri-drivers libxkbcommon-x11-devel \
  freetype-devel libjpeg-turbo-devel fontconfig-devel xz-devel \
  xcb-util-devel xcb-util-renderutil-devel xcb-util-keysyms-devel xcb-util-image-devel xcb-util-wm-devel xorg-x11-util-macros \
  java-1.8.0-openjdk expat-devel lpsolve-devel libuuid-devel curl-devel \
  texlive-collection-latexrecommended texlive-pdftex texlive-latex-bin tex-preview pandoc dvisvgm \
  devtoolset-8-gcc devtoolset-8-gcc-c++ devtoolset-8-gcc-gfortran devtoolset-8-binutils cmake3 \
 && ln -s /usr/bin/cmake3 /usr/bin/cmake && ln -s /usr/bin/ctest3 /usr/bin/ctest \
 && sed -i "s|override_install_langs|#override_install_langs|g" /etc/yum.conf && yum -y reinstall glibc-common && localedef -i en_US -f UTF-8 C.UTF-8 \
 && mkdir -p /usr/share/texlive/texmf-local/tex/latex/ && cd /usr/share/texlive/texmf-local/tex/latex \
 && curl -fsSL http://mirrors.ctan.org/macros/latex/contrib/anyfontsize.zip | bsdtar -xf- && mktexlsr

ENV LANG="en_US.UTF-8" MAKEFLAGS="-j8"
ENV PATH=/opt/rh/devtoolset-8/root/usr/bin:$PATH LD_LIBRARY_PATH=/opt/rh/devtoolset-8/root/usr/lib64:/opt/rh/devtoolset-8/root/usr/lib
WORKDIR /usr/local/src

# swig
RUN curl -fSsL https://github.com/swig/swig/archive/v4.0.2.tar.gz | tar xz && cd swig-4.0.2 \
 && ./autogen.sh && ./configure --without-alllang && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && swig -version && cd - && rm -r swig*

# ipopt
RUN curl -fsSL https://bitbucket.org/petsc/pkg-metis/get/v5.1.0-p10.tar.bz2 | tar xj && cd petsc-pkg-metis-* && make config shared=1 > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r petsc*
RUN curl -fsSL https://github.com/coin-or-tools/ThirdParty-Mumps/archive/releases/1.6.2.tar.gz | tar xz && cd ThirdParty-Mumps-releases-1.6.2 && ./get.Mumps && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r ThirdParty*
RUN curl -fsSL https://github.com/coin-or/Ipopt/archive/releases/3.14.6.tar.gz | tar xz && cd Ipopt-releases-3.14.6 && ./configure --prefix=/usr/local --without-hsl --disable-java && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Ipopt*

# bonmin
RUN curl -fsSL https://www.coin-or.org/download/source/Bonmin/Bonmin-1.8.8.tgz | tar xz && cd Bonmin-1.8.8 && ./configure --prefix=/usr/local --with-ipopt-lib="$(pkg-config --libs ipopt)" --with-ipopt-incdir="/usr/local/include/coin-or/" && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Bonmin*

# dlib
RUN curl -L https://github.com/davisking/dlib/archive/v19.24.tar.gz | tar xz && cd dlib-19.24 \
 && mkdir build && cd build && cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release .. && make install > /dev/null 2>&1 && cd - && rm -r dlib*

# cminpack
RUN curl -fsSL https://github.com/devernay/cminpack/archive/v1.3.8.tar.gz | tar xz && cd cminpack-1.3.8 \
 && cmake -DCMINPACK_LIB_INSTALL_DIR=lib -DBUILD_SHARED_LIBS=ON -DBUILD_EXAMPLES=OFF -DCMINPACK_PRECISION=d . && make install > /dev/null 2>&1 && cd - && rm -r cminpack*

# nlopt
RUN curl -fSsL https://github.com/stevengj/nlopt/archive/v2.7.1.tar.gz | tar xz && cd nlopt-2.7.1 \
 && cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_MATLAB=OFF . && make install > /dev/null 2>&1 && cd - && rm -r nlopt*

# hdf5
RUN curl -fsSL https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.12/hdf5-1.12.2/src/hdf5-1.12.2.tar.bz2 | tar xj && cd hdf5-1.12.2 && ./configure --enable-cxx --prefix=/usr/local --disable-tests --disable-tools && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r hdf5*

# eigen
RUN curl -fsSL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.bz2 | tar xj && cd eigen-3.4.0 \
 && mkdir build && cd build && cmake .. && make install > /dev/null 2>&1 && cd - && rm -r eigen*

# spectra
RUN curl -fsSL https://github.com/yixuan/spectra/archive/v1.0.1.tar.gz | tar xz && cd spectra-1.0.1 && cmake . && make install > /dev/null 2>&1 && cd - && rm -r spectra*

# glog
RUN curl -fsSL https://github.com/google/glog/archive/v0.6.0.tar.gz | tar xz && cd glog-0.6.0 && cmake . && make install > /dev/null 2>&1 && cd - && rm -rf glog*

# ceres
RUN curl -fsSL https://github.com/ceres-solver/ceres-solver/archive/2.1.0.tar.gz | tar xz && cd ceres-solver-2.1.0 \
 && cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=ON -DGFLAGS=OFF . && make install > /dev/null 2>&1 && cd - && rm -r ceres*

# tbb
RUN curl -fSsL https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.5.0.tar.gz | tar xz && cd oneTBB-2021.5.0 && cmake -DTBB_TEST=OFF . && make install > /dev/null 2>&1 && cd - && rm -r oneTBB*

# primesieve
RUN curl -fsSL https://github.com/kimwalisch/primesieve/archive/v7.9.tar.gz | tar xz && cd primesieve-7.9 && cmake -DBUILD_STATIC_LIBS=OFF . && make install > /dev/null 2>&1 && cd - && rm -r primesieve*

# appimagetool
RUN wget -q https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage \
 && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root \
 && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool --version

# openssl
RUN curl -fSsL https://www.openssl.org/source/openssl-1.1.1n.tar.gz | tar xz && cd openssl-1.1.1n \
 && ./config no-ssl2 no-shared -fPIC --prefix=/opt/ssl && make depend > /dev/null 2>&1 && make > /dev/null 2>&1 && make install_sw > /dev/null 2>&1

# python
RUN curl -fSsL https://www.python.org/ftp/python/3.10.4/Python-3.10.4.tgz | tar xz && cd Python-3.10.4 \
 && ./configure --enable-shared CFLAGS="-I/opt/ssl/include" LDFLAGS="-Wl,-rpath /usr/local/lib -L/opt/ssl/lib" && make > /dev/null 2>&1 && make install > /dev/null 2>&1 \
 && python3 -V && python3 -c "import ssl" && cd - && rm -r Python*

# pip
RUN curl -fsSLO https://bootstrap.pypa.io/get-pip.py && python3 get-pip.py && pip install -U pip wheel setuptools cython pybind11
RUN curl -fSsL https://github.com/numpy/numpy/archive/v1.21.6.tar.gz | tar xz && cd numpy-1.21.6 \
 && python3 setup.py build ${MAKEFLAGS} > /dev/null 2>&1 && python3 setup.py install > /dev/null 2>&1 && cd - && python3 -c "import numpy" && cd - && rm -r numpy*
RUN pip install -U pythran && curl -fSsL https://pypi.python.org/packages/source/s/scipy/scipy-1.7.3.tar.gz | tar xz && cd scipy-1.7.3 \
 && python3 setup.py build ${MAKEFLAGS} > /dev/null 2>&1 && python3 setup.py install > /dev/null 2>&1 && cd - && python3 -c "import scipy.linalg" && cd - && rm -r scipy*
RUN pip install lxml pandas matplotlib ipython sphinx numpydoc dill --upgrade

# sundials
RUN curl -fSsL https://github.com/LLNL/sundials/archive/v5.8.0.tar.gz | tar xz && cd sundials-5.8.0 \
 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DEXAMPLES_INSTALL=OFF \
    -DENABLE_LAPACK=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_SIZE=32 -DCMAKE_INSTALL_LIBDIR=lib .. \
 && make install > /dev/null 2>&1 && cd ../.. && rm -r sundials*

# fmilib
RUN curl -fSsL https://github.com/modelon-community/fmi-library/archive/2.4.tar.gz | tar xz && cd fmi-library-2.4 \
 && cd build && cmake -DFMILIB_INSTALL_PREFIX=/usr/local .. && make install > /dev/null 2>&1 && cd ../.. && rm -r fmi*

# assimulo
RUN curl -fSsL https://github.com/modelon-community/Assimulo/archive/Assimulo-3.2.9.tar.gz | tar xz && cd Assimulo-Assimulo-3.2.9 \
 && python3 setup.py install --extra-fortran-link-flags="-shared" --sundials-home=/usr/local --lapack-home=/usr/lib64 --blas-home=/usr/lib64 > /dev/null 2>&1 \
 && cd - && python3 -c "import assimulo; import assimulo.solvers" && rm -r Assimulo*

# pyfmi
RUN curl -fSsL https://github.com/modelon-community/PyFMI/archive/PyFMI-2.9.7.tar.gz | tar xz && cd PyFMI-PyFMI-2.9.7 \
 && python3 setup.py install --fmil-home=/usr/local > /dev/null 2>&1 && cd - && python3 -c "import pyfmi" && rm -r PyFMI*

# boost
RUN curl -fSsL https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.bz2 | tar xj && cd boost_1_79_0 \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python3 \
 && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system ${MAKEFLAGS} install --with-locale --with-filesystem --with-serialization > /dev/null 2>&1 && cd - && rm -r boost*

# pagmo
RUN curl -fsSL https://github.com/esa/pagmo2/archive/refs/tags/v2.18.0.tar.gz | tar xz && cd pagmo2-2.18.0 && cmake -DPAGMO_WITH_EIGEN3=ON . && make install > /dev/null 2>&1 && cd - && rm -r pagmo*

# ninja
RUN curl -fSsL https://github.com/ninja-build/ninja/archive/refs/tags/v1.10.2.tar.gz | tar xz && cd ninja-1.10.2 && cmake -DCMAKE_BUILD_TYPE=Release . && make install > /dev/null 2>&1

# wayland
RUN pip install meson && curl -fSsL https://wayland.freedesktop.org/releases/wayland-1.20.0.tar.xz | tar xJ && cd wayland-1.20.0 && meson build --buildtype=release -Ddocumentation=false && ninja -C build install > /dev/null 2>&1 && cd - && rm -r wayland*

# qt
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qtbase-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qtbase-everywhere-src-5.15.4 && ./configure -xcb -no-mimetype-database -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent -nomake tests -nomake examples && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtbase*
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qtsvg-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qtsvg-everywhere-src-5.15.4 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtsvg*
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qtx11extras-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qtx11extras-everywhere-src-5.15.4 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtx11extras*
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qttools-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qttools-everywhere-src-5.15.4 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qttools*
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qtxmlpatterns-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qtxmlpatterns-everywhere-src-5.15.4 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtxmlpatterns*
RUN curl -fSsL https://download.qt.io/official_releases/qt/5.15/5.15.4/submodules/qtwayland-everywhere-opensource-src-5.15.4.tar.xz | tar xJ && cd qtwayland-everywhere-src-5.15.4 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtwayland*

# qwt
RUN curl -fSsL http://downloads.sourceforge.net/qwt/qwt-6.2.0.tar.bz2 | tar xj && cd qwt-6.2.0 \
 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qwt*

# paraview
RUN curl -fSsL https://paraview.org/files/v5.10/ParaView-v5.10.1.tar.xz | tar xJ && cd ParaView-v5.10.1 \
 && mkdir build && cd build \
 && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX=/usr/local/ \
  -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-8/root/usr/bin/gfortran \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF -DPARAVIEW_USE_VTKM=OFF \
  -DOPENGL_opengl_LIBRARY=/usr/lib64/libGL.so .. \
 && make install > /dev/null 2>&1 && cd ../.. && rm -r ParaView*

# openturns
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout v1.19 \
 && cmake -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" \
    -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1 && cd - && rm -r openturns*

# otmorris
RUN git clone https://github.com/openturns/otmorris.git && cd otmorris && git checkout v0.13 \
 && cmake -DUSE_SPHINX=OFF -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DUSE_SPHINX=OFF . \
 && make install > /dev/null 2>&1 && cd - && rm -r otmorris*

# otfmi
RUN pip install git+https://github.com/openturns/otfmi@v0.15 && python3 -c "import otfmi"

# openmodelica
RUN git clone -b v1.19.0 --depth 1 https://github.com/OpenModelica/OpenModelica.git && cd OpenModelica \
 && git submodule update --init --recursive OMCompiler/3rdParty \
 && cd OMCompiler && autoconf && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 \
 && cd ../.. && rm -r OpenModelica*

# devel user
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
