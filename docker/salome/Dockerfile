
FROM debian:buster
MAINTAINER jschueller

RUN apt-get -y update && apt-get -y install git g++ libxml2-dev doxygen graphviz libgraphviz-dev libhdf5-dev libcppunit-dev curl libboost-chrono-dev libboost-filesystem-dev libboost-date-time-dev libboost-regex-dev libboost-thread-dev libboost-signals-dev libboost-locale-dev libssl-dev pkgconf libqt5svg5-dev libqt5xmlpatterns5-dev libqt5webkit5-dev libqt5x11extras5-dev qtbase5-dev qttools5-dev qtwebengine5-dev liblapack-dev mesa-common-dev libfreetype6-dev tk-dev libxmu-dev libx11-dev libxi-dev qt5-default sudo libcminpack-dev net-tools xvfb procps xterm gdb vim swig cmake libpython3-dev python3-pip libqwt-qt5-dev locales python3-pyqt5 occt-misc libocct-visualization-dev libocct-draw-dev libocct-data-exchange-dev libtbb-dev pyqt5-dev-tools pyqt5-dev

# locale
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8

WORKDIR /usr/local/src
ENV MAKEFLAGS "-j8"
ENV LD_LIBRARY_PATH /usr/local/lib/

# omniorb
RUN curl -L http://downloads.sourceforge.net/omniorb/omniORB-4.2.3.tar.bz2|tar xj && cd omniORB-4.2.3 && ./configure --with-openssl=/usr && make && make install
RUN curl -L http://downloads.sourceforge.net/omniorb/omniORBpy/omniORBpy-4.2.3/omniORBpy-4.2.3.tar.bz2|tar xj && cd omniORBpy-4.2.3 && ./configure && make && make install

# openturns
RUN git clone https://github.com/openturns/openturns.git && cd openturns && git checkout 1.14 \
 && cmake -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" -DSWIG_COMPILE_FLAGS="-O1" -DUSE_SPHINX=OFF . && make install > /dev/null 2>&1

# libbatch
RUN git clone https://git.salome-platform.org/gitpub/tools/libbatch.git && cd libbatch && git checkout V2_4_1 && cmake . && make install

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
RUN git clone https://git.salome-platform.org/gitpub/modules/gui.git && cd gui && git checkout V9_3_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DSALOME_USE_VTKVIEWER=OFF -DSALOME_BUILD_DOC=OFF -DSALOME_USE_PVVIEWER=OFF -DSALOME_USE_VTKVIEWER=OFF -DSALOME_USE_PLOT2DVIEWER=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local -DSALOME_CMAKE_DEBUG=ON . && make install

# salome-yacs
ADD yacs_py37.patch /tmp
RUN git clone https://git.salome-platform.org/gitpub/modules/yacs.git && cd yacs && git checkout V9_3_0 && patch -p1 -i /tmp/yacs_py37.patch && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DKERNEL_ROOT_DIR=/home/devel/local -DGUI_ROOT_DIR=/home/devel/local -DSALOME_BUILD_DOC=OFF -DSALOME_YACS_USE_DSC=OFF -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install && cp -v src/py2yacsgui/*.hxx /home/devel/local/include

# py2cpp
RUN git clone https://github.com/jschueller/py2cpp.git && cd py2cpp && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# ydefx
RUN git clone https://github.com/jschueller/ydefx.git && cd ydefx && git checkout V9_3_0 && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DSalomeKERNEL_DIR=/home/devel/local/salome_adm/cmake_files -DCMAKE_INSTALL_PREFIX=/home/devel/local . &&  make install

# adao_interface
RUN git clone https://github.com/jschueller/adao_interface.git && cd adao_interface && cmake -DCONFIGURATION_ROOT_DIR=/home/devel/src/configuration -DPY2CPP_ROOT_DIR=/home/devel/local -DCMAKE_INSTALL_PREFIX=/home/devel/local . && make install

# finalize install
ENV LD_LIBRARY_PATH /usr/local/lib/:/home/devel/local/lib/:/home/devel/local/lib/salome
ENV PYTHONPATH /usr/local/lib/python3.7/site-packages:/home/devel/local/lib/python3.7/site-packages/salome:/home/devel/local/bin/salome

# salome virtual app
ADD config_appli.xml /tmp/config_appli.xml
ADD salome_context.cfg /tmp/salome_context.cfg
RUN python3 /home/devel/local/bin/salome/appli_gen.py  --prefix=/home/devel --config=/tmp/config_appli.xml
RUN /home/devel/salome info

# try test
RUN sudo ln -sf /usr/bin/python3 /usr/bin/python 
ENV KERNEL_ROOT_DIR=/home/devel/local GUI_ROOT_DIR=/home/devel/local YDEFX_ROOT_DIR=/home/devel/local 
RUN /home/devel/salome test
