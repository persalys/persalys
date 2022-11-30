Persalys
========
OpenTURNS-based GUI for uncertainty treatment

Installation
------------
- Windows:

  * requirements: Windows>=10
  * to use system OpenGL libs remove the provided opengl32.dll

- Linux:

  * requirements: glibc>=2.17, X11/Wayland, OpenGL>=3.2, FUSE
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

- CMake >=3.13
- OpenTURNS >=1.20
- otfmi >=0.14 (optional, for FMI support)
- otmorris >=0.9 (optional, for Morris support)
- Qt >=5
- Qwt >=6
- Python >=3.5
- SWIG >=3
- Boost.locale/filesystem
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- ParaView >=5.8 (optional, for visualization)

Compilation::

    git clone https://git.phimeca.com/ot-edf/otgui.git persalys
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

- Python console menu translation::

    lupdate -verbose lib/src/view/PyConsole/ -ts lib/src/view/PyConsole/resources/PyConsole_msg_fr.ts  -no-obsolete
    linguist lib/src/view/PyConsole/resources/PyConsole_msg_fr.ts

  publish ``PyConsole_msg_fr.qm`` in ``translations/``
