FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN aurman -Syu --noconfirm --noedit --pgp_fetch mingw-w64-qwt
RUN aurman -Syu --noconfirm --noedit --pgp_fetch mingw-w64-paraview510 > /dev/null 2>&1
RUN aurman -Syu --noconfirm --noedit --pgp_fetch mingw-w64-mesa

ENV ARCH=x86_64 PYMAJMIN=310
ENV MINGW_PREFIX /usr/${ARCH}-w64-mingw32

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns && git checkout v1.20.1 \
  && ${ARCH}-w64-mingw32-cmake \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DUSE_SPHINX=OFF . \
  && make > /dev/null 2>&1 && sudo make install > /dev/null \
  && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libOT.dll ${MINGW_PREFIX}/Lib/site-packages/openturns/*.pyd \
  && rm -r /tmp/openturns*

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris && git checkout v0.13 \
  && ${ARCH}-w64-mingw32-cmake \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin . \
  && make > /dev/null 2>&1 && sudo make install > /dev/null \
  && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libotmorris.dll ${MINGW_PREFIX}/Lib/site-packages/otmorris/*.pyd \
  && rm -r /tmp/otmorris

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py && sudo mv /tmp/otfmi.py ${MINGW_PREFIX}/lib/python${PYMAJMIN}/otfmi.py

