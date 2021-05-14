FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN aurman -Syu --pgp_fetch --noconfirm --noedit mingw-w64-qwt
RUN aurman -Syu --noconfirm --noedit --pgp_fetch mingw-w64-paraview > /dev/null 2>&1
RUN aurman -Syu --noconfirm --noedit --pgp_fetch mingw-w64-mesa

ENV ARCH=x86_64 PYMAJMIN=38
ENV MINGW_PREFIX /usr/${ARCH}-w64-mingw32

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns && git checkout v1.16 \
  # PR1691: LinearCalibration: Handle nan in deltaTheta
  && git cherry-pick ad2aa99217ba1cde1b0cadb00ebb0766b4e5f4d6 \
  # PR1766: Path: Dont use dladdr on win32
  && git cherry-pick 657359b318d6ddf57a20d7b556d9441c6f2b48fd \
  # PR1802: Dlib: Fix build with clang
  && git cherry-pick ce334848c4a8eb4cd47b2186627b15d379a143ee --strategy-option ours \
  && ${ARCH}-w64-mingw32-cmake \
  -DCMINPACK_LIBRARIES="cminpack::cminpack" \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 -DUSE_SPHINX=OFF -DUSE_HMAT=OFF -DUSE_SPECTRA=OFF . \
  && make > /dev/null 2>&1 && sudo make install \
  && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libOT.dll ${MINGW_PREFIX}/Lib/site-packages/openturns/*.pyd \
  && rm -r /tmp/openturns*

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris && git checkout v0.11 \
  && ${ARCH}-w64-mingw32-cmake \
  -DPYTHON_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} -DPYTHON_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPYTHON_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin -DPYTHON_SITE_PACKAGES=Lib/site-packages . \
  && make && sudo make install \
  && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libotmorris.dll ${MINGW_PREFIX}/Lib/site-packages/otmorris/*.pyd \
  && rm -r /tmp/otmorris

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py && sudo mv /tmp/otfmi.py ${MINGW_PREFIX}/lib/python${PYMAJMIN}/otfmi.py

# https://bugreports.qt.io/browse/QTBUG-80578
RUN sudo sed -i 's|#ifndef __TBB_profiling_H|#if !defined(__TBB_profiling_H) \&\& !defined(Q_MOC_RUN)|g' ${MINGW_PREFIX}/include/tbb/tbb_profiling.h || :
