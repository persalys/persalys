FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN aurman -Syu --pgp_fetch --noconfirm --noedit mingw-w64-qwt
RUN aurman -S --noconfirm --noedit --pgp_fetch mingw-w64-paraview58
RUN aurman -S --noconfirm --noedit --pgp_fetch mingw-w64-mesa

ENV ARCH=x86_64 PYMAJMIN=37
ENV MINGW_PREFIX /usr/${ARCH}-w64-mingw32

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns && git checkout v1.15 \
  && ${ARCH}-w64-mingw32-cmake-py${PYMAJMIN} -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DUSE_SPHINX=OFF . \
  && make > /dev/null 2>&1 && sudo make install && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libOT.dll ${MINGW_PREFIX}/Lib/site-packages/openturns/*.pyd

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris && git checkout v0.8 \
  && ${ARCH}-w64-mingw32-cmake-py${PYMAJMIN} -DPYTHON_SITE_PACKAGES=Lib/site-packages . \
  && make && sudo make install && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libotmorris.dll ${MINGW_PREFIX}/Lib/site-packages/otmorris/*.pyd

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py \
 && sudo mv /tmp/otfmi.py ${MINGW_PREFIX}/lib/python${PYMAJMIN}/otfmi.py

RUN sudo pacman -Sy unzip --noconfirm

# https://bugreports.qt.io/browse/QTBUG-80578
RUN sudo sed -i 's|#ifndef __TBB_profiling_H|#if !defined(__TBB_profiling_H) \&\& !defined(Q_MOC_RUN)|g' ${MINGW_PREFIX}/include/tbb/tbb_profiling.h || :

