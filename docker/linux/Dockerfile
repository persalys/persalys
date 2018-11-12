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
RUN git clone https://github.com/beltoforion/muparser.git && cd muparser && git checkout v2.2.5 && ./configure --disable-samples && make && make install > /dev/null 2>&1

# swig
RUN yum -y install pcre-devel
RUN curl -SsL http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz | tar xz && cd swig-3.0.12 \
 && ./configure --without-alllang && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && swig -version

# cmake
RUN curl -SsL https://cmake.org/files/v3.12/cmake-3.12.1.tar.gz | tar xz && cd cmake-3.12.1 \
 && ./bootstrap --parallel=8 && make install > /dev/null 2>&1

# nlopt
RUN curl -SsL https://github.com/stevengj/nlopt/archive/v2.5.0.tar.gz | tar xz && cd nlopt-2.5.0 \
 && cmake -DINSTALL_LIB_DIR=/usr/local/lib -DBUILD_SHARED_LIBS=ON -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF -DNLOPT_GUILE=OFF -DNLOPT_MATLAB=OFF . && make install > /dev/null 2>&1

# tbb
RUN git clone https://github.com/wjakob/tbb.git && cd tbb && cmake -DTBB_BUILD_TESTS=OFF . && make install > /dev/null 2>&1

# appimagetool
RUN wget -q https://github.com/AppImage/AppImageKit/releases/download/10/appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage \
 && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root \
 && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool -h

# glibc 2.14
RUN curl -SsL http://ftp.gnu.org/gnu/glibc/glibc-2.14.tar.gz | tar xz && cd glibc-2.14 \
 && mkdir build && cd build && ../configure --prefix=/opt/glibc-2.14 && make > /dev/null 2>&1 && make install > /dev/null 2>&1

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
RUN yum -y install zlib-devel bzip2-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel gpg libffi-devel
RUN curl -SsL https://www.openssl.org/source/openssl-1.0.2p.tar.gz | tar xz && cd openssl-1.0.2p \
 && ./config no-ssl2 no-shared -fPIC --prefix=/usr/local/ssl && make depend > /dev/null 2>&1 && make > /dev/null 2>&1 && make install_sw > /dev/null 2>&1
RUN curl -SsL https://www.python.org/ftp/python/3.7.1/Python-3.7.1.tgz | tar xz && cd Python-3.7.1 \
 && ./configure --enable-shared --enable-unicode=ucs4 CFLAGS="-I/usr/local/ssl/include" LDFLAGS="-Wl,-rpath /usr/local/lib -L/usr/local/ssl/lib" \
 && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && python3 -V && python3 -c "import ssl"

# pip
RUN curl -sSLO https://bootstrap.pypa.io/get-pip.py && python3 get-pip.py \
 && pip install -U pip wheel setuptools && pip install scipy matplotlib ipython --upgrade

# sundials
RUN curl -kL https://computation.llnl.gov/projects/sundials/download/sundials-2.7.0.tar.gz | tar xz && cd sundials-2.7.0 \
 && mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF \
    -DEXAMPLES_INSTALL=OFF -DLAPACK_ENABLE=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON -DSUNDIALS_INDEX_TYPE=int32_t .. && make install > /dev/null 2>&1

# fmilib
RUN wget -q https://jmodelica.org/FMILibrary/FMILibrary-2.0.3-src.zip && unzip FMILibrary-2.0.3-src.zip && cd FMILibrary-2.0.3 \
 && mkdir build && cd build && cmake -DFMILIB_INSTALL_PREFIX=/usr/local .. && make install > /dev/null 2>&1

# pyfmi
RUN pip install lxml cython \
 && wget -q https://pypi.io/packages/source/A/Assimulo/Assimulo-2.9.zip && unzip Assimulo-2.9.zip && cd Assimulo-2.9 && python3 setup.py install --extra-fortran-link-flags="-shared" --sundials-home=/usr/local \
 && sed -i 's|print("Could not find GLIMDA")|pass|g' /usr/local/lib/python*/site-packages/assimulo/solvers/glimda.py \
 && cd .. && python3 -c "import assimulo; import assimulo.solvers" \
 && wget -q https://pypi.io/packages/source/P/PyFMI/PyFMI-2.4.0.zip && unzip PyFMI-2.4.0.zip && cd PyFMI-2.4 \
 && curl -SsL https://github.com/conda-forge/pyfmi-feedstock/raw/master/recipe/get_temp_location-keyerror.patch | patch -p1 \
 && python3 setup.py install --fmil-home=/usr/local && cd .. && python3 -c "import pyfmi"

# sphinx
RUN yum -y install pandoc dvipng
RUN pip install sphinx numpydoc --upgrade

# boost
# https://svn.boost.org/trac10/ticket/11120
RUN ln -s /usr/local/include/python3.7m /usr/local/include/python3.7
RUN curl -SsL https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.bz2 | tar xj && cd boost_1_68_0 \
 && ./bootstrap.sh --with-toolset=gcc --with-icu --with-python=/usr/local/bin/python3 && ./b2 -q variant=release address-model=64 architecture=x86 debug-symbols=off threading=multi runtime-link=shared link=shared toolset=gcc --layout=system -j8 install > /dev/null 2>&1

# X11
RUN yum -y install libX11-devel libXt-devel libXext-devel chrpath libXrender-devel gtk2-devel dbus-devel libSM-devel libICE-devel libXrandr-devel xorg-x11-server-Xvfb
RUN yum -y install mesa-libGL-devel mesa-libEGL-devel mesa-libGLU-devel mesa-libGLw-devel
RUN yum -y install freetype-devel libjpeg-turbo-devel libxcb-devel xcb-util xcb-util-devel fontconfig-devel xz-devel
RUN xvfb-run -h

# qt
RUN curl -SsL https://download.qt.io/archive/qt/5.11/5.11.2/single/qt-everywhere-src-5.11.2.tar.xz | tar xJ && cd qt-everywhere-src-5.11.2 \
 && ./configure -skip qt3d -skip qtandroidextras -skip qtcanvas3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtdeclarative -skip qtgamepad -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtnetworkauth -skip qtpurchasing -skip qtquickcontrols -skip qtquickcontrols2 -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qttranslations -skip qtvirtualkeyboard -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebsockets -skip qtwebview \
 -confirm-license -opensource -prefix /usr/local -headerdir /usr/local/include/qt -archdatadir /usr/local/lib/qt -datadir /usr/local/share/qt -silent \
 -nomake tests -nomake examples && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# qwt
RUN curl -SsL http://downloads.sourceforge.net/qwt/qwt-6.1.3.tar.bz2 | tar xj && cd qwt-6.1.3 \
 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# paraview
RUN curl -SsL http://paraview.org/files/v5.6/ParaView-v5.6.0.tar.gz | tar xz && cd ParaView-v5.6.0 \
  && mkdir build && cd build && cmake \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DVTK_USE_64BIT_IDS=OFF \
  -DVTK_NO_PYTHON_THREADS=OFF \
  -DVTK_PYTHON_FULL_THREADSAFE=ON \
  -DPARAVIEW_ENABLE_PYTHON=ON \
  -DBUILD_DOCUMENTATION=OFF \
  -DPARAVIEW_ENABLE_EMBEDDED_DOCUMENTATION=OFF \
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
  .. && make install > /dev/null 2>&1

# openturns
RUN yum -y install blas-devel lapack-devel libxml2-devel
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout v1.12 \
 && cmake -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 . && make install > /dev/null 2>&1

# otmorris
RUN git clone https://github.com/openturns/otmorris.git && cd otmorris && git checkout v0.5 && git cherry-pick 721afcf4db5b5d372a3b9af99f730eff8a447dc0 \
 && cmake -DUSE_SPHINX=OFF -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DPYTHON_EXECUTABLE=/usr/local/bin/python3 . && make install > /dev/null 2>&1

# otfmi
RUN git clone https://github.com/openturns/otfmi.git && cd otfmi && git checkout v0.2 && python3 setup.py install

# locale
RUN localedef -i en_US -f UTF-8 C.UTF-8
ENV LANG "C.UTF-8"

# openmodelica
RUN yum -y install java-1.8.0-openjdk expat-devel lpsolve-devel libuuid-devel libtool gettext zip
RUN git clone --recursive https://github.com/OpenModelica/OMCompiler.git && cd OMCompiler && git checkout v1.12.0 \
 && git submodule sync && git submodule update --init --recursive \
 && git cherry-pick 63848be39fe6ff0bb7096c8fad3f68b8078286db \
 && autoconf && ./configure --prefix=/usr/local && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# devel user
RUN yum -y install sudo
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
