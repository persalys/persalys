Persalys Module
===============
Qt GUI module for OpenTURNS

Dependencies
------------
- CMake >=3.8
- OpenTURNS >=1.14
- Qt >=5
- Qwt >=6
- Python >=3.2
- Boost.locale/filesystem/regex
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- otfmi (optional, for FMI support)
- ParaView >=5.8 (optional, for visualization)
- otmorris >=0.7 (optional, for Morris support)

Installation
------------
- The Qwt library found must be linked to the qt libs found (else adjust QWT_LIBRARY)
- otfmi must be importable at configure time, so both openturns & otfmi should be reachable via PYTHONPATH or a default location
- ParaView must be linked to the same qt libs, built with PARAVIEW_BUILD_QT_GUI=ON and PARAVIEW_PLUGIN_ENABLE_BagPlotViewsAndFilters=ON (default)
- On windows OpenGL software rendering is provided for graphic widgets with mesa, if you wish to use system-provided GL libs you can remove opengl32.dll
- To override the default language you can set LANG=en/fr environment variable, on windows see persalys.vbs

Compile Persalys::

    git clone http://git.phimeca.lan/ot-edf/persalys.git
    cd persalys
    mkdir -p build && cd build
    cmake \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DOpenTURNS_DIR=$PWD/../../openturns/build/install/lib/cmake/openturns \
      -DParaView_DIR=$PWD/../../paraview/build/install/lib/cmake/paraview \
      -DOTMORRIS_DIR=$PWD/../../otmorris/build/install/lib/cmake/otmorris \
      ..
    make install

Run Persalys::

    persalys.sh

Translation::

    lupdate -verbose lib/ -ts translations/persalys_fr.ts -no-obsolete
    linguist translations/persalys_fr.ts

Environment variables:

- PERSALYS_HTML_DOCUMENTATION_PATH: override path to the html documentation
- PERSALYS_NO_GL: if defined, this disables OpenGL (used for ParaView widgets) support for troubleshooting
- PERSALYS_CALIBRATION_ENGINE: if defined to "adao" and adao support is enabled, this switches to adao for calibration computations

