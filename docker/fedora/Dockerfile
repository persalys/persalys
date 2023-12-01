FROM fedora:rawhide

RUN yum install -y libxml2-devel cmake gcc-c++ cminpack-devel coin-or-Bonmin-devel swig python3-devel flexiblas-devel 
RUN yum install -y git ceres-solver-devel coin-or-Ipopt-devel tbb-devel libmpc-devel boost-devel hdf5-devel bison flex primesieve-devel

WORKDIR /tmp
ENV OPENTURNS_NUM_THREADS=1 MAKEFLAGS="-j10"
RUN git  clone --depth 1 -b misc https://github.com/jschueller/openturns.git && cd openturns && cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DSWIG_COMPILE_FLAGS="-O0" -DCMAKE_CXX_FLAGS="-Wall -Wextra -D_GLIBCXX_ASSERTIONS" . && make install
#RUN cd openturns && ctest -R pyinstallcheck_BlockIndep --timeout 250
#RUN cd openturns && ctest -R pyinstallcheck_StudentFactory_std --timeout 250
#RUN cd openturns && ctest -E cppcheck --output-on-failure --timeout 250 --schedule-random ${MAKEFLAGS}


RUN yum install -y qt5-qttools-devel qt5-qtsvg-devel qwt-qt5-devel
RUN yum install -y xorg-x11-server-Xvfb
RUN yum install -y clang

# RUN find  /usr/include  -name qwt_plot.h
# RUN ls -l /usr/include/qwt5/qwt/qwt_plot.h
# /usr/include/qt5/qwt/qwt_plot.h

# RUN git   clone --depth 1 https://github.com/persalys/persalys.git
#RUN git clone --depth 1 https://git.phimeca.com/persalys/persalys.git
#RUN sed -i "s|qwt-qt4|qt5/qwt|g" persalys/cmake/FindQwt.cmake
#RUN sed -i "s|krigingResult.getBasisCollection()\[0\]|krigingResult.getBasis()|g" persalys/lib/src/base/KrigingAnalysis.cxx
# RUN sed -i "s|NAMES qwt_plot.h|NAMES qwt/qwt_plot.h|g" persalys/cmake/FindQwt.cmake
# RUN sed -i "/HINTS/d" persalys/cmake/FindQwt.cmake
# RUN cd persalys && git diff
# RUN cd persalys && cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DCMAKE_CXX_FLAGS="-Wall -Wextra -D_GLIBCXX_ASSERTIONS" -DOpenTURNS_DIR=/tmp/openturns/install/lib/cmake/openturns . && make install
