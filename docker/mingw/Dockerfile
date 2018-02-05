
FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN yaourt -S --noconfirm mingw-w64-qwt
RUN yaourt -S --noconfirm mingw-w64-paraview

ENV ARCH i686
ENV WINEARCH win32
ENV WINEPATH=/usr/${ARCH}-w64-mingw32/bin
RUN rm -rf ${HOME}/.wine*
ENV PYTHONHOME /usr/${ARCH}-w64-mingw32/
ENV PYTHONPATH /usr/${ARCH}-w64-mingw32/lib/python27
ENV MAKEFLAGS "-j8"

# openturns
RUN git clone https://github.com/openturns/openturns.git /tmp/openturns && cd /tmp/openturns \
  && git checkout 1.10 \
  && CXXFLAGS="-D_hypot=hypot" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=/usr/${ARCH}-w64-mingw32/include/python27 \
  -DPYTHON_LIBRARY=/usr/${ARCH}-w64-mingw32/lib/libpython27.dll.a \
  -DPYTHON_EXECUTABLE=/usr/${ARCH}-w64-mingw32/bin/python27.exe \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j8" \
  -DUSE_TBB=OFF . \
  && make && sudo make install

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris \
  && CXXFLAGS="-D_hypot=hypot" ${ARCH}-w64-mingw32-cmake -DUSE_SPHINX=OFF \
  -DPYTHON_INCLUDE_DIR=/usr/${ARCH}-w64-mingw32/include/python27 \
  -DPYTHON_LIBRARY=/usr/${ARCH}-w64-mingw32/lib/libpython27.dll.a \
  -DPYTHON_EXECUTABLE=/usr/${ARCH}-w64-mingw32/bin/python27.exe . \
  -DPYTHON_SITE_PACKAGES=Lib/site-packages \
  && make && sudo make install

# fake otfmi
RUN echo "__version__= '0.0'" > /tmp/otfmi.py \
 && sudo mv /tmp/otfmi.py /usr/${ARCH}-w64-mingw32/lib/python27/otfmi.py

