
FROM debian:11
MAINTAINER jschueller

RUN apt-get -y update && apt-get -y install file gdb git g++ cmake python3 python3-pyqt5 pyqt5-dev pyqt5-dev-tools python3-pip sip-dev swig libxml2-dev doxygen graphviz libgraphviz-dev libhdf5-dev libcppunit-dev curl libboost-chrono-dev libboost-filesystem-dev libboost-date-time-dev libboost-program-options-dev libboost-regex-dev libboost-thread-dev libboost-locale-dev libssl-dev pkgconf libqt5svg5-dev libqt5xmlpatterns5-dev libqt5webkit5-dev libqt5x11extras5-dev qtbase5-dev qttools5-dev qtwebengine5-dev liblapack-dev mesa-common-dev libfreetype6-dev tk-dev libxmu-dev libx11-dev libxi-dev sudo libcminpack-dev net-tools xvfb procps xterm gdb vim locales libtbb-dev libffi-dev libgdbm-dev libncurses5-dev libexpat1-dev libpcre3-dev wget bison flex texinfo libqwt-qt5-dev

# locale
RUN echo "en_US.UTF-8 UTF-8\nfr_FR.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8

WORKDIR /usr/local/src
ENV MAKEFLAGS "-j8"
ENV LD_LIBRARY_PATH /usr/local/lib/

# pip
RUN pip3 install psutil packaging numpy==1.21.1 scipy==1.6.2

# omniorb
RUN curl -L https://downloads.sourceforge.net/omniorb/omniORB-4.2.5.tar.bz2|tar xj && cd omniORB-4.2.5 && ./configure --with-openssl=/usr && make > /dev/null 2>&1 && make install && cd - && rm -r omniORB*
RUN curl -L https://downloads.sourceforge.net/omniorb/omniORBpy/omniORBpy-4.2.5/omniORBpy-4.2.5.tar.bz2|tar xj && cd omniORBpy-4.2.5 && ./configure && make > /dev/null 2>&1 && make install && cd - && rm -r omniORBpy*

# opencascade
RUN curl -L "https://git.dev.opencascade.org/gitweb/?p=occt.git;a=snapshot;h=refs/tags/SALOME_9_13_V7_8_0;sf=tgz" | tar xz && cd occt* && cmake -DBUILD_DOC_Overview=OFF . && make install > /dev/null 2>&1 && cd - && rm -r occt*

# openturns
RUN git clone -b v1.24rc1 --depth 1 https://github.com/openturns/openturns.git && cd openturns \
 && cmake -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DSWIG_COMPILE_FLAGS="-O1" -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1 && cd - && rm -r openturns*

# libbatch
RUN git clone -b V2_5_0 --depth 1 https://git.salome-platform.org/gitpub/tools/libbatch.git && cd libbatch && cmake . && make install > /dev/null 2>&1 && cd - && rm -r libbatch

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
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/configuration.git && cd configuration && sed -i "s|NO_SYSTEM_ENVIRONMENT_PATH||g" cmake/SalomeMacros.cmake

# salome-kernel
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/kernel.git && cd kernel && cmake -DSALOME_BUILD_DOC=OFF -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSALOME_USE_LIBBATCH=ON -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1

# salome-boostrap
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/salome_bootstrap.git && cd salome_bootstrap && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1
ENV PYTHONPATH /home/devel/src/salome_bootstrap/__SALOME_BOOTSTRAP__/:$PYTHONPATH

# salome-gui
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/gui.git && cd gui && sed -i "137iFIND_PACKAGE(SalomeGraphviz)" CMakeLists.txt && echo "set_target_properties (LightApp PROPERTIES EXCLUDE_FROM_ALL TRUE)" >> src/LightApp/CMakeLists.txt && sed -i "2135,2160d" src/LightApp/LightApp_Application.cxx && git diff && cmake -LAH -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSALOMEBOOTSTRAP_ROOT_DIR=/home/devel/src/salome_bootstrap -DKERNEL_ROOT_DIR=/home/devel/local -DSALOME_USE_VTKVIEWER=OFF -DSALOME_BUILD_DOC=OFF -DSALOME_USE_PVVIEWER=OFF -DSALOME_USE_PV3DVIEWER=OFF -DSALOME_USE_VTKVIEWER=OFF -DSALOME_USE_PLOT2DVIEWER=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local -DSALOME_CMAKE_DEBUG=ON . && make install && cd - && rm -rf gui*

# salome-yacs
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/yacs.git && cd yacs && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DGUI_ROOT_DIR=/home/devel/local -DSALOME_BUILD_DOC=OFF -DSALOME_YACS_USE_DSC=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r yacs*

# py2cpp
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/py2cpp.git && cd py2cpp && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r py2cpp*

# ydefx
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/ydefx.git && cd ydefx && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files -DCMAKE_INSTALL_PREFIX=/home/devel/local -DKERNEL_ROOT_DIR=/home/devel/local . && make install > /dev/null 2>&1 && cd - && rm -r ydefx*

# adao
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/adao.git && cd adao && CURRENT_SOFTWARE_INSTALL_DIR=/usr/local cmake -DADAO_BUILD_DOC=OFF -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration . && make > /dev/null 2>&1 && sudo make install > /dev/null 2>&1 && cd - && sudo rm -r adao*

# adao_interface
RUN git clone -b V9_13_0 --depth 1 https://github.com/SalomePlatform/adao_interface.git && cd adao_interface && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DPY2CPP_ROOT_DIR=/home/devel/local -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install > /dev/null 2>&1 && cd - && sudo rm -r adao*

# finalize install
ENV LD_LIBRARY_PATH /usr/local/lib/:/home/devel/local/lib/:/home/devel/local/lib/salome
ENV PYTHONPATH /usr/local/lib/python3.9/site-packages:/usr/local/lib/python3.9/site-packages:/home/devel/local/lib/python3.9/site-packages/salome:/home/devel/local/bin/salome:/home/devel/local/lib/salome/

# salome virtual app
ADD config_appli.xml /tmp/config_appli.xml
ADD salome_context.cfg /tmp/salome_context.cfg
RUN python3 /home/devel/local/bin/salome/appli_gen.py --prefix=/home/devel/appli --config=/tmp/config_appli.xml
RUN /home/devel/appli/salome info

# try test
RUN sudo ln -sf /usr/local/bin/python3 /usr/local/bin/python
ENV KERNEL_ROOT_DIR=/home/devel/local GUI_ROOT_DIR=/home/devel/local YDEFX_ROOT_DIR=/home/devel/local
RUN /home/devel/appli/salome test --output-on-failure
