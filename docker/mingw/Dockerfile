
FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN rm -rf ${HOME}/.wine*

# https://github.com/docker/for-linux/issues/208
RUN aurman -S --noconfirm --noedit mingw-w64-qt5-base-nostatx

RUN aurman -S --noconfirm --noedit mingw-w64-qwt
RUN aurman -Syu --noconfirm --noedit mingw-w64-paraview

ENV ARCH x86_64
ENV WINEARCH win64
ENV WINEPATH /usr/${ARCH}-w64-mingw32/bin
ENV PYTHONHOME /usr/${ARCH}-w64-mingw32/
ENV PYTHONPATH /usr/${ARCH}-w64-mingw32/lib/python27
ENV MAKEFLAGS -j8

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns \
  && git checkout 1.11 \
  && CXXFLAGS="-D_hypot=hypot" ${ARCH}-w64-mingw32-cmake \
  -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=/usr/${ARCH}-w64-mingw32/include/python27 \
  -DPYTHON_LIBRARY=/usr/${ARCH}-w64-mingw32/lib/libpython27.dll.a \
  -DPYTHON_EXECUTABLE=/usr/${ARCH}-w64-mingw32/bin/python27.exe \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DUSE_TBB=OFF . \
  && make && sudo make install
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/bin/libOT.dll
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/Lib/site-packages/openturns/*.pyd

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris \
  && git checkout v0.4 \
  && CXXFLAGS="-D_hypot=hypot" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=/usr/${ARCH}-w64-mingw32/include/python27 \
  -DPYTHON_LIBRARY=/usr/${ARCH}-w64-mingw32/lib/libpython27.dll.a \
  -DPYTHON_EXECUTABLE=/usr/${ARCH}-w64-mingw32/bin/python27.exe . \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  && make && sudo make install
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/bin/libotmorris.dll
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/Lib/site-packages/otmorris/*.pyd

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py \
 && sudo mv /tmp/otfmi.py /usr/${ARCH}-w64-mingw32/lib/python27/otfmi.py

