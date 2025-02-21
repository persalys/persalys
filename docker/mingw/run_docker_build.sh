#!/bin/sh

set -x -e

uid=$1
gid=$2

cd /tmp

mkdir -p build && cd build
MOD_PREFIX=$PWD/install
CXXFLAGS="-Wall -Wextra -Wpedantic -Wshadow -Werror -DPERSALYS_NSIS" ${ARCH}-w64-mingw32-cmake -DUSE_SALOME=OFF \
  -DCMAKE_INSTALL_PREFIX=${MOD_PREFIX} \
  -DCMAKE_LINKER_TYPE=LLD \
  -DPython_INCLUDE_DIR=${MINGW_PREFIX}/include/python${PYMAJMIN} \
  -DPython_LIBRARY=${MINGW_PREFIX}/lib/libpython${PYMAJMIN}.dll.a \
  -DPython_EXECUTABLE=/usr/bin/${ARCH}-w64-mingw32-python${PYMAJMIN}-bin \
  -DSWIG_COMPILE_FLAGS="-Wno-unused-parameter -Wno-shadow" \
  -DCMAKE_UNITY_BUILD=ON -DCMAKE_UNITY_BUILD_BATCH_SIZE=32 /io
make install
${ARCH}-w64-mingw32-strip --strip-unneeded ${MOD_PREFIX}/bin/*.dll ${MOD_PREFIX}/Lib/site-packages/persalys/*.pyd
make tests
cp ${MOD_PREFIX}/bin/*.dll ${MOD_PREFIX}/Lib/site-packages/persalys/
cp ${MINGW_PREFIX}/bin/*.dll ${MOD_PREFIX}/bin
OPENTURNS_NUM_THREADS=2 WINEPATH="${MINGW_PREFIX}/bin;${MOD_PREFIX}/bin" xvfb-run -s "-screen 0 1024x768x24" ctest --output-on-failure --timeout 200 ${MAKEFLAGS}

cp -r ${MINGW_PREFIX}/lib/qt/plugins/{platforms,imageformats} ${MOD_PREFIX}/lib
cp -rv ${MINGW_PREFIX}/bin/paraview*/plugins/BagPlotViewsAndFilters ${MOD_PREFIX}/bin
cp /usr/${ARCH}-w64-mingw32/etc/openturns/openturns.conf ${MOD_PREFIX}/Lib/site-packages/persalys/
VERSION=`cat /io/VERSION`
cp /io/distro/windows/* .
bsdtar -xf persalys-doc.zip

# bundle installer
cd /tmp
PYVER=`${ARCH}-w64-mingw32-python${PYMAJMIN}-bin -V|sed "s|.*Python \([0-9\.]*\).*|\1|g"`
mkdir -p python_root/Lib/site-packages && cd python_root
curl -fSsL https://www.python.org/ftp/python/${PYVER}/python-${PYVER}-embed-amd64.zip | bsdtar -xf-
echo 'Lib\\site-packages' >> python${PYMAJMIN}._pth
curl -fSsL https://anaconda.org/conda-forge/vs2015_runtime/14.16.27012/download/win-64/vs2015_runtime-14.16.27012-h30e32a0_2.tar.bz2 | tar xj
cp -r /usr/${ARCH}-w64-mingw32/Lib/site-packages/openturns Lib/site-packages
cp /usr/${ARCH}-w64-mingw32/bin/*.dll Lib/site-packages/openturns
cp /usr/${ARCH}-w64-mingw32/etc/openturns/openturns.conf Lib/site-packages/openturns
rm Lib/site-packages/openturns/{libvtk,libboost,Qt,python}*.dll
cp -rv /usr/${ARCH}-w64-mingw32/Lib/site-packages/otmorris Lib/site-packages

curl -fSsL https://anaconda.org/conda-forge/pyfmi/2.14.0/download/win-64/pyfmi-2.14.0-py312h1a27103_1.conda | bsdtar -x && tar -xf pkg-pyfmi-2.14.0-py312h1a27103_1.tar.zst
curl -fSsL https://anaconda.org/conda-forge/assimulo/3.5.2/download/win-64/assimulo-3.5.2-py312h5d292b3_1.conda | bsdtar -x && tar -xf pkg-assimulo-3.5.2-py312h5d292b3_1.tar.zst
curl -fSsL https://anaconda.org/conda-forge/sundials/7.1.1/download/win-64/sundials-7.1.1-h62746c2_0.conda | bsdtar -x && tar -xf pkg-sundials-7.1.1-h62746c2_0.tar.zst
cp -r Library/bin/*.dll . && rm -r Library info pkg-*.zst info-*.zst
cd Lib
curl -fSsL https://anaconda.org/conda-forge/otfmi/0.16.5/download/noarch/otfmi-0.16.5-pyhff2d567_0.conda | bsdtar -x && tar -xf pkg-otfmi-0.16.5-pyhff2d567_0.tar.zst
cd site-packages
curl -fsSL https://pypi.io/packages/py3/p/pip/pip-24.1.1-py3-none-any.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/py2.py3/w/wheel/wheel-0.34.2-py2.py3-none-any.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/py2.py3/s/six/six-1.16.0-py2.py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py2.py3/p/pytz/pytz-2021.3-py2.py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py2.py3/p/python_dateutil/python_dateutil-2.8.2-py2.py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py3/d/dill/dill-0.3.8-py3-none-any.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/py3/s/setuptools/setuptools-49.2.0-py3-none-any.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/cp312/n/numpy/numpy-1.26.4-cp312-cp312-win_amd64.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/cp312/s/scipy/scipy-1.13.1-cp312-cp312-win_amd64.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/cp312/p/pandas/pandas-2.2.2-cp312-cp312-win_amd64.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py2.py3/o/openpyxl/openpyxl-3.1.2-py2.py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py3/e/et_xmlfile/et_xmlfile-1.1.0-py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py3/j/jinja2/jinja2-3.1.4-py3-none-any.whl | bsdtar -xf-
curl -fsSL https://pypi.io/packages/cp312/m/markupsafe/MarkupSafe-2.1.5-cp312-cp312-win_amd64.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py3/p/pythonfmu/pythonfmu-0.6.3-py3-none-any.whl | bsdtar -xf-
curl -fSsL https://pypi.io/packages/py3/n/nopip/nopip-0.2.2-py3-none-any.whl | bsdtar -xf-
cd ../..
mkdir Scripts && echo -e 'import sys\nfrom pip import main\nsys.exit(main())\n' > Scripts/pip.py && echo -e 'python %~dp0pip.py %*' > Scripts/pip.bat
cd /tmp/build
makensis -DMODULE_PREFIX=${MOD_PREFIX} -DMODULE_VERSION=${VERSION} -DPYTHON_PREFIX=/tmp/python_root -DPYBASEVER=${PYMAJMIN:0:1}.${PYMAJMIN:1} -DARCH=${ARCH} installer_bundle.nsi

# copy to host with same permission
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp persalys*.exe /io
  sudo chown ${uid}:${gid} /io/persalys*.exe
fi
