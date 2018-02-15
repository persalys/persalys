OTGui Module
============
Qt GUI module for OpenTURNS

Dependencies
------------
- OpenTURNS >=1.10
- Qt >=4
- Qwt >=6
- Python >=2.7, < 3
- Boost.regex
- Boost.filesystem
- Boost.chrono
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- otfmi (optional, for FMI support)
- ParaView >=5.4.0 (optional, for visualisation)
- otmorris >=0.3 (optional, for Morris support)

Installation
------------
- Python libs found must be version 2.7 as well as interpreter (else adjust PYTHON_INCLUDE_DIR, PYTHON_LIBRARY)
- The Qwt library found must be linked to the qt libs found (else adjust QWT_LIBRARY, or use USE_QT5=OFF)
- otfmi must be importable at configure time, so both openturns & otfmi should be reachable via PYTHONPATH or a default location
- ParaView must be linked to the same qt libs, and built with PARAVIEW_BUILD_QT_GUI=ON and VTK_PYTHON_FULL_THREADSAFE=ON

Compile OTGui::

    git clone http://git.phimeca.lan/ot-edf/otgui.git
    cd otgui
    mkdir -p build && cd build
    cmake \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DOpenTURNS_DIR=$PWD/../../openturns/build/install/lib/cmake/openturns \
      -DParaView_DIR=$PWD/../../paraview/build/install/lib/cmake/paraview \
      -DOTMORRIS_DIR=$PWD/../../otmorris/build/install/lib/cmake/otmorris \
      ..
    make install

Run OTGui::

    gui_launch.sh

Translation::

    lupdate-qt4 -verbose lib/ -ts translations/otgui_fr.ts -no-obsolete
    linguist-qt4 translations/otgui_fr.ts
