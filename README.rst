OTGui Module
============
Qt GUI module for OpenTURNS

Dependencies
------------
- OpenTURNS >= 1.8
- Qt >= 4
- Qwt >= 6
- Python >= 2.7, < 3
- SalomeYACS (optional, for YACS support)
- Sphinx >= 1.1 (optional, for doc)
- Numpydoc (optional, for doc)

Installation
------------
- Python libs found must be version 2.7 as well as interpreter (else adjust PYTHON_INCLUDE_DIR, PYTHON_LIBRARY)
- The Qwt library found must be linked to the qt libs found (else adjust QWT_LIBRARY, or use USE_QT5=OFF)

Compile OpenTURNS::

    git clone https://github.com/openturns/openturns.git
    cd openturns
    git checkout 1.8
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