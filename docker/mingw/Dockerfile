FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN aurman -Syu --noconfirm --noedit   mingw-w64-qwt
RUN aurman -Syu --noconfirm --noedit   mingw-w64-paraview


ENV ARCH=x86_64 PYMAJMIN=37
ENV MINGW_PREFIX /usr/${ARCH}-w64-mingw32

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns \
  && git checkout 1.13 \
  && ${ARCH}-w64-mingw32-cmake-py${PYMAJMIN} -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DUSE_SPHINX=OFF -DUSE_TBB=OFF . \
  && make && sudo make install && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libOT.dll ${MINGW_PREFIX}/Lib/site-packages/openturns/*.pyd

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris \
  && git checkout v0.6 \
  && ${ARCH}-w64-mingw32-cmake-py${PYMAJMIN} -DPYTHON_SITE_PACKAGES=Lib/site-packages . \
  && make && sudo make install && sudo ${ARCH}-w64-mingw32-strip --strip-unneeded ${MINGW_PREFIX}/bin/libotmorris.dll ${MINGW_PREFIX}/Lib/site-packages/otmorris/*.pyd

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py \
 && sudo mv /tmp/otfmi.py ${MINGW_PREFIX}/lib/python${PYMAJMIN}/otfmi.py

RUN sudo pacman -S unzip --noconfirm
