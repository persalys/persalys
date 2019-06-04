Persalys Module
===============
Qt GUI module for OpenTURNS

Dependencies
------------
- OpenTURNS >=1.13
- Qt >=5
- Qwt >=6
- Python >=3.2
- Boost.locale
- Boost.filesystem
- Boost.regex
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- otfmi (optional, for FMI support)
- ParaView >=5.4.0 (optional, for visualisation)
- otmorris >=0.6 (optional, for Morris support)

Installation
------------
- Python libs found must of same version as interpreter (else adjust PYTHON_INCLUDE_DIR, PYTHON_LIBRARY)
- The Qwt library found must be linked to the qt libs found (else adjust QWT_LIBRARY)
- otfmi must be importable at configure time, so both openturns & otfmi should be reachable via PYTHONPATH or a default location
- ParaView must be linked to the same qt libs, and built with PARAVIEW_BUILD_QT_GUI=ON

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
- PERSALYS_NO_GL: if defined, this disables OpenGL (Paraview widgets) support for troubleshooting

