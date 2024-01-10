
FROM debian:10
MAINTAINER jschueller

RUN apt-get -y update && apt-get -y install git g++ libxml2-dev doxygen graphviz libgraphviz-dev libhdf5-dev libcppunit-dev curl libboost-chrono-dev libboost-filesystem-dev libboost-date-time-dev libboost-program-options-dev libboost-regex-dev libboost-thread-dev libboost-signals-dev libboost-locale-dev libssl-dev pkgconf libqt5svg5-dev libqt5xmlpatterns5-dev libqt5webkit5-dev libqt5x11extras5-dev qtbase5-dev qttools5-dev qtwebengine5-dev liblapack-dev mesa-common-dev libfreetype6-dev tk-dev libxmu-dev libx11-dev libxi-dev qt5-default sudo libcminpack-dev net-tools xvfb procps xterm gdb vim locales libtbb-dev libffi-dev libgdbm-dev libncurses5-dev libexpat1-dev libpcre3-dev wget bison flex texinfo libqwt-qt5-dev

# locale
RUN echo "en_US.UTF-8 UTF-8\nfr_FR.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8

WORKDIR /usr/local/src
ENV MAKEFLAGS "-j8"
ENV LD_LIBRARY_PATH /usr/local/lib/

# swig
RUN curl -fSsL http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz | tar xz && cd swig-3.0.12 \
 && ./configure --without-alllang && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && swig -version && cd - && rm -r swig*

# python
RUN curl -fSsL https://www.python.org/ftp/python/3.6.5/Python-3.6.5.tgz | tar xz && cd Python-3.6.5 \
 && ./configure --enable-shared && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && python3 -V && python3 -c "import ssl" && cd - && rm -r Python*

# pip
RUN curl -fsSLO https://bootstrap.pypa.io/pip/3.6/get-pip.py && python3 get-pip.py && pip install -U pip wheel setuptools numpy==1.15.1 scipy==0.19.1 psutil packaging

# gdb
RUN curl -fSsL https://ftp.gnu.org/gnu/gdb/gdb-8.2.1.tar.gz |tar xz && cd gdb-8.2.1 && ./configure --disable-tui --with-python=no && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cd - && rm -r gdb*

# omniorb
RUN curl -L https://downloads.sourceforge.net/omniorb/omniORB-4.2.5.tar.bz2|tar xj && cd omniORB-4.2.5 && ./configure --with-openssl=/usr && make > /dev/null 2>&1 && make install && cd - && rm -r omniORB*
RUN curl -L https://downloads.sourceforge.net/omniorb/omniORBpy/omniORBpy-4.2.5/omniORBpy-4.2.5.tar.bz2|tar xj && cd omniORBpy-4.2.5 && ./configure && make > /dev/null 2>&1 && make install && cd - && rm -r omniORBpy*

# cmake
RUN curl -fSsL https://cmake.org/files/v3.16/cmake-3.16.3.tar.gz | tar xz && cd cmake-3.16.3 && ./bootstrap --parallel=8 && make install > /dev/null 2>&1 && cd - && rm -r cmake*

# opencascade
RUN curl -L "https://git.dev.opencascade.org/gitweb/?p=occt.git;a=snapshot;h=refs/tags/V7_5_3;sf=tgz" | tar xz && cd occt-V7_5_3 && cmake -DBUILD_DOC_Overview=OFF . && make install > /dev/null 2>&1 && cd - && rm -r occt*

# openturns
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout v1.21.3 \
 && cmake -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DSWIG_COMPILE_FLAGS="-O1" -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1 && cd - && rm -r openturns*

# libbatch
RUN git clone https://git.salome-platform.org/gitpub/tools/libbatch.git && cd libbatch && git checkout V2_4_6 && cmake . && make install > /dev/null 2>&1 && cd - && rm -r libbatch

# sip
RUN curl -fSsL https://downloads.sourceforge.net/pyqt/sip/sip-4.19.13/sip-4.19.13.tar.gz | tar xz && cd sip-4.19.13 && mkdir build && cd build && python3 ../configure.py  && make > /dev/null 2>&1 && make install > /dev/null 2>&1 && cp /usr/local/include/python3.6m/sip.h /usr/local/include && cd - && rm -r sip*

# pyqt
RUN curl -fSsL http://downloads.sourceforge.net/pyqt/PyQt5/PyQt5_gpl-5.11.3.tar.gz |tar xz && cd PyQt5_gpl-5.11.3 \
 && sed -i "s|PyQt5.sip|sip|g" configure.py && python3 configure.py --verbose --confirm-license \
 && make > /dev/null 2>&1 && make install -j1 > /dev/null 2>&1 \
 && python3 -c "from PyQt5 import QtCore; print(QtCore.PYQT_VERSION_STR)" && cd - && rm -r PyQt*

# uname -p => unknown
RUN cd /tmp && sudo mv /bin/uname /bin/uname.orig && echo -n "#!/bin/sh\nuname.orig \"\$@\"|sed 's|unknown|x86_64|g'\n" > uname && chmod a+rx ./uname && sudo mv ./uname /bin/uname && uname  -p

# appimagetool
RUN wget -q https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage \
 && chmod a+rx appimagetool-x86_64.AppImage \
 && ./appimagetool-x86_64.AppImage --appimage-extract \
 && chmod -R a+rx squashfs-root \
 && ln -s /usr/local/src/squashfs-root/AppRun /usr/bin/appimagetool \
 && appimagetool -h

# devel user
RUN useradd -m -d /home/devel -u 1000 -U -G users,tty -s /bin/bash devel
RUN echo 'devel ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER devel
ENV USER devel
RUN mkdir /home/devel/src
WORKDIR /home/devel/src
RUN git config --global user.email devel@a.b && git config --global user.name devel

# salome-configuration
RUN git clone https://git.salome-platform.org/gitpub/tools/configuration.git && cd configuration && git checkout V9_11_0 && sed -i "s|NO_SYSTEM_ENVIRONMENT_PATH||g" cmake/SalomeMacros.cmake

# salome-kernel
RUN git clone https://git.salome-platform.org/gitpub/modules/kernel.git && cd kernel && git checkout V9_11_0 && cmake -DSALOME_BUILD_DOC=OFF -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSALOME_USE_LIBBATCH=ON -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r kernel*

# salome-gui
RUN git clone https://git.salome-platform.org/gitpub/modules/gui.git && cd gui && git checkout V9_11_0 && sed -i "130iFIND_PACKAGE(SalomeGraphviz)" CMakeLists.txt && echo "set_target_properties (LightApp PROPERTIES EXCLUDE_FROM_ALL TRUE)" >> src/LightApp/CMakeLists.txt && sed -i "2054,2078d" src/LightApp/LightApp_Application.cxx && git diff && cmake -LAH -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DSALOME_USE_VTKVIEWER=OFF -DSALOME_BUILD_DOC=OFF -DSALOME_USE_PVVIEWER=OFF -DSALOME_USE_PV3DVIEWER=OFF -DSALOME_USE_VTKVIEWER=OFF -DSALOME_USE_PLOT2DVIEWER=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local -DSALOME_CMAKE_DEBUG=ON . && make install && cd - && rm -rf gui*

# salome-yacs
RUN git clone https://git.salome-platform.org/gitpub/modules/yacs.git && cd yacs && git checkout V9_11_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DGUI_ROOT_DIR=/home/devel/local -DSALOME_BUILD_DOC=OFF -DSALOME_YACS_USE_DSC=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r yacs*

# py2cpp
RUN git clone https://git.salome-platform.org/gitpub/tools/py2cpp.git && cd py2cpp && git checkout V9_11_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r py2cpp*

# ydefx
RUN git clone https://git.salome-platform.org/gitpub/tools/ydefx.git && cd ydefx && git checkout V9_11_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files -DCMAKE_INSTALL_PREFIX=/home/devel/local -DKERNEL_ROOT_DIR=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r ydefx*

# adao
RUN git clone https://git.salome-platform.org/gitpub/modules/adao.git && cd adao && git checkout V9_11_0 && CURRENT_SOFTWARE_INSTALL_DIR=/usr/local cmake -DADAO_BUILD_DOC=OFF -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration . && make > /dev/null 2>&1 && sudo make install > /dev/null 2>&1 && cd - && sudo rm -r adao*

# adao_interface
RUN git clone https://git.salome-platform.org/gitpub/tools/adao_interface.git && cd adao_interface && git checkout V9_7_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DPY2CPP_ROOT_DIR=/home/devel/local -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && sudo rm -r adao*

# finalize install
ENV LD_LIBRARY_PATH /usr/local/lib/:/home/devel/local/lib/:/home/devel/local/lib/salome
ENV PYTHONPATH /usr/local/lib/python3.6/site-packages:/usr/local/lib/python3.6/site-packages:/home/devel/local/lib/python3.6/site-packages/salome:/home/devel/local/bin/salome:/home/devel/local/lib/salome/

# salome virtual app
ADD config_appli.xml /tmp/config_appli.xml
ADD salome_context.cfg /tmp/salome_context.cfg
RUN python3 /home/devel/local/bin/salome/appli_gen.py --prefix=/home/devel/appli --config=/tmp/config_appli.xml
RUN /home/devel/appli/salome info

# try test
RUN sudo ln -sf /usr/local/bin/python3 /usr/local/bin/python
ENV KERNEL_ROOT_DIR=/home/devel/local GUI_ROOT_DIR=/home/devel/local YDEFX_ROOT_DIR=/home/devel/local 
RUN /home/devel/appli/salome test --output-on-failure
