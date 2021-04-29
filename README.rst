Persalys
========
OpenTURNS-based GUI for uncertainty treatment

Installation
------------
- Windows:

  * requirements: Windows>=7, KB2533623 and KB2999226 updates
  * to use system OpenGL libs remove the provided opengl32.dll

- Linux:

  * requirements: glibc>=2.17, X11/Wayland, OpenGL>=3.2
  * no install required, just run the .AppImage

Environment variables:

- LANG: can be set to en/fr to override the language on Windows, see persalys.vbs
- LANGUAGE: same on Linux
- PERSALYS_HTML_DOCUMENTATION_PATH: override path to the html documentation
- PERSALYS_NO_GL: if defined, this disables OpenGL (used for ParaView widgets)
- PERSALYS_CALIBRATION_ENGINE: if defined to "adao" and adao support is enabled
  this switches to adao for calibration computations

Development
-----------
Dependencies:

- CMake >=3.8
- OpenTURNS >=1.17
- Qt >=5
- Qwt >=6
- Python >=3.2
- Boost.locale/filesystem
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- otfmi (optional, for FMI support)
- ParaView >=5.8 (optional, for visualization)
- otmorris >=0.9 (optional, for Morris support)

Compilation::

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

Launch::

    persalys.sh

Translation::

    lupdate -verbose lib/ -ts translations/persalys_fr.ts -no-obsolete
    linguist translations/persalys_fr.ts

