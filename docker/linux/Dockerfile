FROM centos:7
MAINTAINER jschueller

# utils
RUN sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-* \
 && sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-* \
 && yum -y install epel-release centos-release-scl \
 && sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-SCLo-scl*.repo \
 && sed -i 's|#[ ]*baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-SCLo-scl*.repo \
 && yum -y install wget git patch zip unzip bzip2 bsdtar perl perl-IPC-Cmd gettext autoconf automake libtool sudo make which file \
  pcre-devel pcre2-devel blas-devel lapack-devel atlas-devel libxml2-devel libmpc-devel mpfr-devel libtree-ldd texinfo \
  zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel gpg libffi-devel openssl-devel \
  libX11-devel libXt-devel libXext-devel libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel \
  xorg-x11-server-Xvfb mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel mesa-dri-drivers libxkbcommon-x11-devel \
  freetype-devel libjpeg-turbo-devel fontconfig-devel xz-devel \
  xcb-util-devel xcb-util-renderutil-devel xcb-util-keysyms-devel xcb-util-image-devel xcb-util-wm-devel xorg-x11-util-macros xcb-util-cursor-devel \
  java-1.8.0-openjdk expat-devel lpsolve-devel libuuid-devel curl-devel \
  texlive-collection-latexrecommended texlive-pdftex texlive-latex-bin tex-preview pandoc dvisvgm \
  devtoolset-11-gcc devtoolset-11-gcc-c++ devtoolset-11-gcc-gfortran devtoolset-11-binutils \
 && sed -i "s|override_install_langs|#override_install_langs|g" /etc/yum.conf && yum -y reinstall glibc-common && localedef -i en_US -f UTF-8 C.UTF-8 \
 && mkdir -p /usr/share/texlive/texmf-local/tex/latex/ && cd /usr/share/texlive/texmf-local/tex/latex \
 && curl -fsSL http://mirrors.ctan.org/macros/latex/contrib/anyfontsize.zip | bsdtar -xf- && mktexlsr

ENV LANG="en_US.UTF-8" MAKEFLAGS="-j8"
ENV PATH=/opt/rh/devtoolset-11/root/usr/bin:$PATH LD_LIBRARY_PATH=/opt/rh/devtoolset-11/root/usr/lib64:/opt/rh/devtoolset-11/root/usr/lib
WORKDIR /usr/local/src

# gdb
RUN curl -fSsL https://ftp.gnu.org/gnu/gdb/gdb-8.3.1.tar.gz |tar xz && cd gdb* && ./configure --with-python=no && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r gdb*

# bison
RUN curl -fsSL https://ftp.gnu.org/gnu/bison/bison-3.8.2.tar.gz | tar xz && cd bison* && ./configure && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r bison*

# cmake
RUN curl -fSsLO https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-linux-x86_64.sh && sh ./cmake-3.28.3-linux-x86_64.sh --skip-license --prefix=/usr/local && rm cmake* && cmake --version

# swig
RUN curl -fSsL https://github.com/swig/swig/archive/v4.3.0.tar.gz | tar xz && cd swig* \
 && cmake . && make install > /dev/null 2>&1 && swig -version && cd - && rm -r swig*

# ipopt
RUN curl -fsSL https://github.com/KarypisLab/GKlib/archive/refs/tags/METIS-v5.1.1-DistDGL-0.5.tar.gz | tar xz && cd GKlib* && curl -L https://github.com/KarypisLab/GKlib/commit/33b8c8bb.patch | patch -p1 && sed -i "s|march=native|march=nocona|g" GKlibSystem.cmake && cmake -DBUILD_SHARED_LIBS=ON . && make install > /dev/null 2>&1 && cd - && rm -r GKlib*
RUN curl -fsSL https://github.com/KarypisLab/METIS/archive/v5.2.1.tar.gz | tar xz && cd METIS-5.2.1 && mkdir -p build/xinclude && sed -i "s|//#define IDXTYPEWIDTH 32|#define IDXTYPEWIDTH 32|g" include/metis.h && sed -i "s|//#define REALTYPEWIDTH 32|#define REALTYPEWIDTH 64|g" include/metis.h && cp include/metis.h include/CMakeLists.txt build/xinclude && echo "target_link_libraries(metis GKlib)" >> libmetis/CMakeLists.txt && sed -i "s|march=native|march=nocona|g" conf/gkbuild.cmake && cmake -DSHARED=ON . && make install > /dev/null 2>&1 && cd - && rm -r METIS*
RUN curl -fsSL https://github.com/coin-or-tools/ThirdParty-Mumps/archive/releases/3.0.7.tar.gz | tar xz && cd ThirdParty-Mumps* && ./get.Mumps && FFLAGS="-O2 -fallow-argument-mismatch" ./configure --prefix=/usr/local && make -j1 > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r ThirdParty*
RUN curl -fsSL https://github.com/coin-or/Ipopt/archive/releases/3.14.17.tar.gz | tar xz && cd Ipopt* && ./configure --prefix=/usr/local --without-hsl --disable-java && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Ipopt*

# bonmin
RUN curl -fsSL https://github.com/coin-or/CoinUtils/archive/refs/tags/releases/2.11.12.tar.gz | tar xz && cd CoinUtils* && ./configure --prefix=/usr/local --with-blas-lib='-lblas' --with-lapack-lib='-llapack' --enable-dependency-linking && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r CoinUtils*
RUN curl -fsSL https://github.com/coin-or/Osi/archive/refs/tags/releases/0.108.10.tar.gz | tar xz && cd Osi* && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Osi*
RUN curl -fsSL https://github.com/coin-or/Clp/archive/refs/tags/releases/1.17.10.tar.gz | tar xz && cd Clp* && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Clp*
RUN curl -fsSL https://github.com/coin-or/Cgl/archive/refs/tags/releases/0.60.9.tar.gz | tar xz && cd Cgl* && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Cgl*
RUN curl -fsSL https://github.com/coin-or/Cbc/archive/refs/tags/releases/2.10.12.tar.gz | tar xz && cd Cbc* && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Cbc*
RUN curl -fsSL https://github.com/coin-or/Bonmin/archive/refs/tags/releases/1.8.9.tar.gz | tar xz && cd Bonmin* && ./configure --prefix=/usr/local --with-ipopt-lib="$(pkg-config --libs ipopt)" --with-ipopt-incdir="/usr/local/include/coin-or/" && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r Bonmin*

# dlib
RUN curl -L https://github.com/davisking/dlib/archive/v20.0.tar.gz | tar xz && cd dlib* \
 && cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -B build . && cd build && make install > /dev/null 2>&1 && cd - && rm -r dlib*

# cminpack
RUN curl -fsSL https://github.com/devernay/cminpack/archive/v1.3.11.tar.gz | tar xz && cd cminpack* \
 && cmake -DCMINPACK_LIB_INSTALL_DIR=lib -DBUILD_SHARED_LIBS=ON -DBUILD_EXAMPLES=OFF -DCMINPACK_PRECISION=d -DUSE_BLAS=OFF . && make install > /dev/null 2>&1 && cd - && rm -r cminpack*

# nlopt
RUN curl -fSsL https://github.com/stevengj/nlopt/archive/v2.10.0.tar.gz | tar xz && cd nlopt* \
 && cmake -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_UNITY_BUILD=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_JAVA=OFF . && make install > /dev/null 2>&1 && cd - && rm -r nlopt*

# hdf5
RUN curl -fsSL https://github.com/HDFGroup/hdf5/archive/hdf5_1.14.6/hdf5-1.14.6.tar.gz | tar xz && cd hdf5* && ./configure --enable-cxx --prefix=/usr/local --disable-tests --disable-tools && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r hdf5*

# nanoflann
RUN curl -fsSL https://github.com/jlblancoc/nanoflann/archive/refs/tags/v1.7.1.tar.gz | tar xz && cd nanoflann* && cmake -DNANOFLANN_BUILD_EXAMPLES=OFF -DNANOFLANN_BUILD_TESTS=OFF . && make install && cd - && rm -r nanoflann*

# eigen
RUN curl -fsSL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.bz2 | tar xj && cd eigen* \
 && cmake -B build . && cd build && make install > /dev/null 2>&1 && cd - && rm -r eigen*

# spectra
RUN curl -fsSL https://github.com/yixuan/spectra/archive/v1.1.0.tar.gz | tar xz && cd spectra* && cmake . && make install > /dev/null 2>&1 && cd - && rm -r spectra*

# glog
RUN curl -fsSL https://github.com/google/glog/archive/v0.7.1.tar.gz | tar xz && cd glog* && cmake . && make install > /dev/null 2>&1 && cd - && rm -rf glog*

# ceres
RUN curl -fsSL https://github.com/ceres-solver/ceres-solver/archive/2.2.0.tar.gz | tar xz && cd ceres* \
 && cmake -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_SHARED_LIBS=ON -DGFLAGS=OFF . && make install > /dev/null 2>&1 && cd - && rm -r ceres*

# tbb
RUN curl -fSsL https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2022.1.0.tar.gz | tar xz && cd oneTBB* && cmake -DTBB_TEST=OFF -DTBB_FILE_TRIM=OFF . && make install > /dev/null 2>&1 && cd - && rm -r oneTBB*

# primesieve
RUN curl -fsSL https://github.com/kimwalisch/primesieve/archive/v12.8.tar.gz | tar xz && cd primesieve* && cmake -DBUILD_STATIC_LIBS=OFF -DWITH_MULTIARCH=OFF -DBUILD_PRIMESIEVE=OFF . && make install > /dev/null 2>&1 && cd - && rm -r primesieve*

# appimagetool
RUN wget -q https://github.com/AppImage/AppImageKit/releases/download/13/obsolete-appimagetool-x86_64.AppImage -O appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool --version

# openssl
RUN curl -fSsL https://www.openssl.org/source/openssl-3.5.0.tar.gz | tar xz && cd openssl* \
 && ./config no-shared -fPIC --prefix=/opt/ssl && make depend > /dev/null 2>&1 && make > /dev/null 2>&1 && make install_sw > /dev/null 2>&1

# libffi
RUN curl -fSsL https://github.com/libffi/libffi/releases/download/v3.4.6/libffi-3.4.6.tar.gz | tar xz && cd libffi* \
 && CFLAGS="-fPIC" ./configure --disable-shared --disable-multi-os-directory --prefix=/opt/ffi && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r libffi*

# python
RUN curl -fSsL https://www.python.org/ftp/python/3.13.3/Python-3.13.3.tgz | tar xz && cd Python* \
 && ./configure --enable-shared --enable-optimizations PKG_CONFIG_PATH=/opt/ffi/lib/pkgconfig/:/opt/ssl/lib64/pkgconfig/ LDFLAGS="-Wl,-rpath /usr/local/lib -L/opt/ssl/lib64 -L/opt/ffi/lib" && make > /dev/null 2>&1 && make install > /dev/null 2>&1 \
 && python3 -V && python3 -c "import ssl; import ctypes" && cd - && rm -r Python*

# pip
RUN pip3 install numpy==2.2.4 scipy==1.15.2 matplotlib ipython sphinx numpydoc dill meson flake8 openpyxl nopip cython setuptools --upgrade --quiet

# sundials
RUN curl -fSsL https://github.com/LLNL/sundials/archive/v7.3.0.tar.gz | tar xz && cd sundials* \
 && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DEXAMPLES_INSTALL=OFF \
    -DENABLE_LAPACK=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_SIZE=32 -DCMAKE_INSTALL_LIBDIR=lib -B build . \
 && cd build && make install > /dev/null 2>&1 && cd ../.. && rm -r sundials*

# fmilib
RUN curl -fSsL https://github.com/modelon-community/fmi-library/archive/3.0.3.tar.gz | tar xz && cd fmi-library* \
 && cmake -DFMILIB_BUILD_STATIC_LIB=OFF -DFMILIB_BUILD_TESTS=OFF -DCMAKE_INSTALL_LIBDIR=lib -B build . && make install -C build > /dev/null 2>&1 && cd - && rm -r fmi*

# assimulo
RUN curl -fSsL https://github.com/modelon-community/Assimulo/archive/Assimulo-3.7.0.tar.gz | tar xz && cd Assimulo* \
 && python3 setup.py install --extra-fortran-link-flags="-shared" --sundials-home=/usr/local --lapack-home=/usr/lib64 --blas-home=/usr/lib64 > /dev/null 2>&1 \
 && cd - && python3 -c "import assimulo; import assimulo.solvers" && rm -r Assimulo*

# pyfmi
RUN curl -fSsL https://github.com/modelon-community/PyFMI/archive/PyFMI-2.17.2.tar.gz | tar xz && cd PyFMI* \
 && python3 setup.py install --fmil-home=/usr/local > /dev/null 2>&1 && cd - && python3 -c "import pyfmi" && rm -r PyFMI*

# libbacktrace
RUN git clone --depth 1 https://github.com/ianlancetaylor/libbacktrace.git && cd libbacktrace \
 && autoreconf -fi && ./configure --enable-shared --disable-static && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r libbacktrace*

# boost
RUN curl -fSsL https://archives.boost.io/release/1.88.0/source/boost_1_88_0.tar.bz2 | tar xj && cd boost* \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python3 \
 && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system ${MAKEFLAGS} install --with-graph --with-chrono --with-serialization --with-program_options --with-filesystem > /dev/null 2>&1 && cd - && rm -r boost*

# pagmo
RUN curl -fsSL https://github.com/esa/pagmo2/archive/refs/tags/v2.19.1.tar.gz | tar xz && cd pagmo* && cmake -DPAGMO_WITH_EIGEN3=ON -DCMAKE_UNITY_BUILD=ON . && make install > /dev/null 2>&1 && cd - && rm -r pagmo*

# ninja
RUN curl -fSsL https://github.com/ninja-build/ninja/archive/refs/tags/v1.11.1.tar.gz | tar xz && cd ninja* && cmake -DCMAKE_BUILD_TYPE=Release . && make install > /dev/null 2>&1 && cd - && rm -r ninja*

# wayland
RUN curl -fSsL https://gitlab.freedesktop.org/wayland/wayland/-/releases/1.23.0/downloads/wayland-1.23.0.tar.xz | tar xJ && cd wayland* \
 && meson build --buildtype=release -Ddocumentation=false && ninja -C build install > /dev/null 2>&1 && cd - && rm -r wayland*

# qt
RUN curl -fSsL https://qt-mirror.dannhauer.de/archive/qt/5.15/5.15.17/submodules/qtbase-everywhere-opensource-src-5.15.17.tar.xz | tar xJ && cd qtbase* && ./configure -xcb -no-mimetype-database -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent -nomake tests -nomake examples && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtbase*
RUN curl -fSsL https://qt-mirror.dannhauer.de/archive/qt/5.15/5.15.17/submodules/qtsvg-everywhere-opensource-src-5.15.17.tar.xz | tar xJ && cd qtsvg* && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtsvg*
RUN curl -fSsL https://qt-mirror.dannhauer.de/archive/qt/5.15/5.15.17/submodules/qtx11extras-everywhere-opensource-src-5.15.17.tar.xz | tar xJ && cd qtx11extras* && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtx11extras*
RUN curl -fSsL https://qt-mirror.dannhauer.de/archive/qt/5.15/5.15.17/submodules/qttools-everywhere-opensource-src-5.15.17.tar.xz | tar xJ && cd qttools* && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qttools*
RUN curl -fSsL https://qt-mirror.dannhauer.de/archive/qt/5.15/5.15.17/submodules/qtwayland-everywhere-opensource-src-5.15.17.tar.xz | tar xJ && cd qtwayland* && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qtwayland*

# qwt
RUN curl -fSsL https://downloads.sourceforge.net/qwt/qwt-6.3.0.tar.bz2 | tar xj && cd qwt* \
 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && sed -i "s|{QWT_INSTALL_PREFIX}/include|{QWT_INSTALL_PREFIX}/include/qwt|g" qwtconfig.pri \
 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r qwt*

# paraview
RUN curl -fSsL https://paraview.org/files/v5.13/ParaView-v5.13.3.tar.xz | tar xJ && cd ParaView* \
 && curl -L https://gitlab.kitware.com/vtk/vtk/merge_requests/11555.patch | patch -p1 -d VTK \
 && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBDIR=lib \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF -DPARAVIEW_PLUGIN_DISABLE_XML_DOCUMENTATION=ON \
  -DOPENGL_opengl_LIBRARY=/usr/lib64/libGL.so -DPARAVIEW_USE_VTKM=OFF -DPARAVIEW_USE_FORTRAN=OFF \
  -DPARAVIEW_PLUGINS_DEFAULT=OFF -DPARAVIEW_PLUGIN_ENABLE_BagPlotViewsAndFilters=ON -B build . \
 && make install -C build > /dev/null 2>&1 && cd - && rm -r ParaView*

# openturns
RUN git clone -b v1.25.1 --depth 1 https://github.com/openturns/openturns.git && cd openturns \
 && cmake -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 \
    -DPython_EXECUTABLE=/usr/local/bin/python3 -DCMAKE_INSTALL_LIBDIR=lib . \
 && make install > /dev/null 2>&1 && cd - && rm -r openturns*

# otmorris
RUN git clone -b v0.18 --depth 1 https://github.com/openturns/otmorris.git && cd otmorris \
 && cmake -DPython_EXECUTABLE=/usr/local/bin/python3 -DCMAKE_INSTALL_LIBDIR=lib . \
 && make install > /dev/null 2>&1 && cd - && rm -r otmorris*

# otfmi
RUN pip3 install git+https://github.com/openturns/otfmi@v0.16.6 --quiet && python3 -c "import otfmi"

# openmodelica
RUN git clone -b v1.25.1 --depth 1 https://github.com/OpenModelica/OpenModelica.git && cd OpenModelica \
 && git submodule update --init --recursive OMCompiler/3rdParty \
 && cmake -DCMAKE_BUILD_TYPE=Release -DOM_USE_CCACHE=OFF -DOM_ENABLE_GUI_CLIENTS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local/ -B build . \
 && make install -C build > /dev/null 2>&1 && cd - && rm -r OpenModelica*

# devel user
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
