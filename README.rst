OTGui Module
============
Qt GUI module for OpenTURNS

Dependencies
------------
- OpenTURNS >= 1.9
- Qt >= 4
- Qwt >= 6
- Python >= 2.7, < 3
- Boost.regex
- Boost.filesystem
- SalomeYACS (optional, for YACS support)
- Sphinx >= 1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- otfmi (optional, for FMI support)

Installation
------------
- Python libs found must be version 2.7 as well as interpreter (else adjust PYTHON_INCLUDE_DIR, PYTHON_LIBRARY)
- The Qwt library found must be linked to the qt libs found (else adjust QWT_LIBRARY, or use USE_QT5=OFF)
- otfmi must be importable at configure time, so both openturns & otfmi should be reachable via PYTHONPATH or a default location

Compile OpenTURNS::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    git checkout 1.9
    mkdir -p build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DUSE_SPHINX=OFF ..
    make install

Compile OTGui::

    git clone http://git.phimeca.lan/ot-edf/otgui.git
    cd otgui
    mkdir -p build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DOpenTURNS_DIR=$PWD/../../build/install/lib/cmake/openturns ..
    make install

Run OTGui::

    gui_launch.sh

Translation::

    lupdate-qt4 -verbose lib/ -ts translations/otgui_fr.ts -no-obsolete
    linguist-qt4 translations/otgui_fr.ts
