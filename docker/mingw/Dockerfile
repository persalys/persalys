FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

# https://github.com/docker/for-linux/issues/208
RUN aurman -Syu --noconfirm --noedit mingw-w64-qt5-base-nostatx

RUN aurman -S --noconfirm --noedit mingw-w64-qwt
RUN aurman -Syu --noconfirm --noedit mingw-w64-paraview

ENV ARCH x86_64
ENV MINGW_PREFIX /usr/${ARCH}-w64-mingw32
ENV PYMAJMIN 36

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns \
  && git checkout v1.12 \
  && CXXFLAGS="-D_hypot=hypot -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake \
  -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DUSE_TBB=OFF . \
  && make && sudo make install
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libOT.dll
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/Lib/site-packages/openturns/*.pyd

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris \
  && git config user.email devel@a.b && git config user.name devel \
  && git checkout v0.5 && git cherry-pick 721afcf4db5b5d372a3b9af99f730eff8a447dc0 \
  && CXXFLAGS="-D_hypot=hypot -D_GLIBCXX_ASSERTIONS" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  && make && sudo make install
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libotmorris.dll
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/Lib/site-packages/otmorris/*.pyd

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py \
 && sudo mv /tmp/otfmi.py ${MINGW_PREFIX}/lib/python${PYMAJMIN}/otfmi.py

