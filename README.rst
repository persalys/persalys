OTGui Module
============

Qt GUI module for OpenTURNS


Installation
============

    git clone https://github.com/openturns/openturns.git
    cd openturns
    git checkout 1.7
    mkdir -p build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DUSE_SPHINX=OFF ..
    make install

    git clone http://git.phimeca.lan/ot-edf/otgui.git
    cd otgui
    git checkout openturns1.7
    mkdir -p build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=$PWD/install -DOpenTURNS_DIR=$PWD/../../build/install/lib/cmake/openturns ..
    make install
        
