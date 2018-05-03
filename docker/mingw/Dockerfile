
FROM openturns/archlinux-mingw:latest
MAINTAINER jschueller

RUN rm -rf ${HOME}/.wine*

# https://github.com/docker/for-linux/issues/208
RUN sudo pacman -Sy --noconfirm wget mingw-w64-libjpeg-turbo mingw-w64-sqlite mingw-w64-libpng mingw-w64-openssl mingw-w64-dbus mingw-w64-harfbuzz mingw-w64-pcre2 mingw-w64-postgresql mingw-w64-mariadb-connector-c
RUN cd /tmp && yaourt -G mingw-w64-qt5-base && cd mingw-w64-qt5-base && makepkg -g
RUN cd /tmp/mingw-w64-qt5-base && sed -i "216i\  wget -c https://github.com/Martchus/qtbase/commit/b47e6c6fc2f5eaf489458fcf5930015f6baf3281.patch && patch -p1 -i b47e6c6fc2f5eaf489458fcf5930015f6baf3281.patch" PKGBUILD && cat PKGBUILD && makepkg
RUN sudo pacman -U /tmp/mingw-w64-qt5-base/mingw-w64-qt5-base*.xz --noconfirm

RUN yaourt -S --noconfirm mingw-w64-qwt
RUN cd /tmp && yaourt -G mingw-w64-cmake && cd mingw-w64-cmake && makepkg && sudo pacman -U mingw-w64-cmake*.xz --noconfirm
RUN yaourt -S --noconfirm mingw-w64-qt5-xmlpatterns mingw-w64-qt5-tools mingw-w64-glew mingw-w64-libtheora mingw-w64-libtiff
RUN yaourt -S --noconfirm mingw-w64-jsoncpp mingw-w64-pugixml mingw-w64-hdf5 mingw-w64-lz4 mingw-w64-cgns mingw-w64-netcdf-cxx-legacy
RUN yaourt -S --noconfirm mingw-w64-paraview
RUN rm -rf ${HOME}/.wine*

ENV ARCH x86_64
ENV WINEARCH win64
ENV WINEPATH /usr/${ARCH}-w64-mingw32/bin
ENV PYTHONHOME /usr/${ARCH}-w64-mingw32/
ENV PYTHONPATH /usr/${ARCH}-w64-mingw32/lib/python27
ENV MAKEFLAGS -j8

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
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/bin/libOT.dll
RUN sudo ${ARCH}-w64-mingw32-strip --strip-unneeded /usr/${ARCH}-w64-mingw32/Lib/site-packages/openturns/*.pyd

# otmorris
RUN git clone https://github.com/openturns/otmorris.git /tmp/otmorris && cd /tmp/otmorris \
  && git checkout v0.3 \
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

