FROM fedora:42

RUN yum install -y cmake git gcc-c++ make boost-devel diffutils patch mold swig python3-devel qwt-qt6-devel xorg-x11-server-Xvfb \
  cminpack-devel eigen3-devel NLopt-devel coin-or-Ipopt-devel tbb-devel libxml2-devel hdf5-devel flexiblas-devel pagmo2-devel
ENV MAKEFLAGS="-j8"

RUN git clone --depth 1 -b v1.25.1 https://github.com/openturns/openturns.git && cd openturns \
 && cmake -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DSWIG_COMPILE_FLAGS="-O1" -DCMAKE_LINKER_TYPE=MOLD . \
 && make install && cd - && rm -r openturns
ENV FLEXIBLAS=NETLIB
