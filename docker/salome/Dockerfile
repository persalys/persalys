
FROM debian:stretch
MAINTAINER jschueller

RUN apt-get -y update && apt-get -y install git g++ libxml2-dev doxygen graphviz libgraphviz-dev libhdf5-dev libcppunit-dev curl libboost-chrono-dev libboost-filesystem-dev libboost-date-time-dev libboost-regex-dev libboost-thread-dev libboost-signals-dev libboost-locale-dev libssl-dev pkgconf libqt5svg5-dev libqt5xmlpatterns5-dev libqt5webkit5-dev libqt5x11extras5-dev qtbase5-dev qttools5-dev qtwebengine5-dev liblapack-dev mesa-common-dev libfreetype6-dev tk-dev libxmu-dev libx11-dev libxi-dev qt5-default sudo libcminpack-dev net-tools xvfb procps xterm gdb vim libffi-dev libgdbm-dev libncurses5-dev libexpat1-dev libpcre3-dev

WORKDIR /usr/local/src
ENV MAKEFLAGS "-j8"
ENV LD_LIBRARY_PATH /usr/local/lib/:/usr/local/lib/salome

# swig
RUN curl -fSsL http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz | tar xz && cd swig-3.0.12 \
 && ./configure --without-alllang && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && swig -version

# python
RUN curl -fSsL https://www.python.org/ftp/python/3.6.9/Python-3.6.9.tgz | tar xz && cd Python-3.6.9 \
 && ./configure --enable-shared && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && python3 -V && python3 -c "import ssl"

# pip
RUN curl -fsSLO https://bootstrap.pypa.io/get-pip.py && python3 get-pip.py \
 && pip install -U pip wheel setuptools && pip install sip --upgrade

# omniorb
RUN curl -L http://downloads.sourceforge.net/omniorb/omniORB-4.2.2.tar.bz2|tar xj && cd omniORB-4.2.2 && ./configure --with-openssl=/usr && make && make install \
 && curl -L http://downloads.sourceforge.net/omniorb/omniORBpy/omniORBpy-4.2.2/omniORBpy-4.2.2.tar.bz2|tar xj && cd omniORBpy-4.2.2 && ./configure && make && make install

# cmake
RUN curl -fSsL https://cmake.org/files/v3.12/cmake-3.12.1.tar.gz | tar xz && cd cmake-3.12.1 && ./bootstrap --parallel=8 && make install > /dev/null 2>&1

# opencascade
RUN curl -L "https://git.dev.opencascade.org/gitweb/?p=occt.git;a=snapshot;h=refs/tags/V7_3_0p3;sf=tgz" | tar xz && cd occt-V7_3_0p3 && cmake -DBUILD_DOC_Overview=OFF . && make install > /dev/null 2>&1

# qwt
RUN curl -fSsL http://downloads.sourceforge.net/qwt/qwt-6.1.4.tar.bz2 | tar xj && cd qwt-6.1.4 \
 && sed -i "s|QWT_INSTALL_PREFIX    = /usr/local/qwt-\$\$QWT_VERSION|QWT_INSTALL_PREFIX = /usr/local|g" qwtconfig.pri \
 && qmake && make > /dev/null 2>&1 && make install > /dev/null 2>&1

# openturns
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout v1.13 \
 && cmake -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" -DCMAKE_CXX_FLAGS="-D_GLIBCXX_ASSERTIONS" -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1

# libbatch
RUN git clone https://git.salome-platform.org/gitpub/tools/libbatch.git && cd libbatch && git checkout V2_4_1 && cmake . && make install

# sip
RUN curl -L https://www.riverbankcomputing.com/static/Downloads/sip/4.19.3/sip-4.19.3.tar.gz | tar xz && cd sip-4.19.3 && mkdir build && cd build && python3 ../configure.py  && make && make install && cp /usr/local/include/python3.6m/sip.h /usr/local/include

# pyqt
RUN curl -fSsL http://prdownloads.sourceforge.net/pyqt/PyQt5/PyQt5_gpl-5.7.1.tar.gz |tar xz && cd PyQt5_gpl-5.7.1 && python3 configure.py --confirm-license && make && make install

# devel user
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
ENV USER devel
RUN mkdir /home/devel/src
WORKDIR /home/devel/src

# salome-configuration
# https://www.salome-platform.org/forum/forum_9/980773415
RUN git clone https://git.salome-platform.org/gitpub/tools/configuration.git && cd configuration && git checkout V9_3_0 && echo "find_package (OpenCASCADE NO_MODULE)" > cmake/FindOpenCASCADE.cmake && sed -i "s|NO_SYSTEM_ENVIRONMENT_PATH||g" cmake/SalomeMacros.cmake

# salome-kernel
RUN git clone https://git.salome-platform.org/gitpub/modules/kernel.git && cd kernel && git checkout V9_3_0 && cmake -DSALOME_BUILD_DOC=OFF -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSALOME_USE_LIBBATCH=ON -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# salome-gui
RUN git clone https://git.salome-platform.org/gitpub/modules/gui.git && cd gui && git checkout V9_3_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DSALOME_USE_VTKVIEWER=OFF -DSALOME_BUILD_DOC=OFF -DSALOME_USE_PVVIEWER=OFF -DSALOME_USE_VTKVIEWER=OFF -DSALOME_USE_PLOT2DVIEWER=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# salome-yacs
RUN git clone https://git.salome-platform.org/gitpub/modules/yacs.git && cd yacs && git checkout V9_3_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DGUI_ROOT_DIR=/home/devel/local -DSALOME_BUILD_DOC=OFF -DSALOME_YACS_USE_DSC=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install && cp -v src/py2yacsgui/*.hxx /home/devel/local/include

# py2cpp
RUN git clone https://github.com/jschueller/py2cpp.git && cd py2cpp && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# ydefx
RUN git clone https://github.com/jschueller/ydefx.git && cd ydefx && git checkout V9_3_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files -DCMAKE_INSTALL_PREFIX=/home/devel/local . &&  make install

# adao_interface
RUN git clone https://github.com/jschueller/adao_interface.git && cd adao_interface && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DPY2CPP_ROOT_DIR=/home/devel/local -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# finalize install
ENV LD_LIBRARY_PATH /usr/local/lib/:/home/devel/local/lib/:/home/devel/local/lib/salome
ENV PYTHONPATH /home/devel/local/lib/python3.6/site-packages/salome:/home/devel/local/bin/salome

# salome virtual app
ADD config_appli.xml /tmp/config_appli.xml
ADD salome_context.cfg /tmp/salome_context.cfg
RUN python3 /home/devel/local/bin/salome/appli_gen.py  --prefix=/home/devel --config=/tmp/config_appli.xml
RUN /home/devel/salome info

# try test
RUN sudo ln -sf /usr/local/bin/python3 /usr/local/bin/python 
ENV KERNEL_ROOT_DIR=/home/devel/local GUI_ROOT_DIR=/home/devel/local YDEFX_ROOT_DIR=/home/devel/local 
RUN cd /home/devel/bin/salome/test/ && ctest -R YDEFX_SampleTest -j8 --output-on-failure

