Persalys
========
OpenTURNS-based GUI for uncertainty treatment

Installation
------------
- Linux:

  * requirements: glibc>=2.5, X11
  * no install required, just run the .AppImage

- Windows:

  * requirements: Windows>=7, KB2533623 and KB2999226 updates
  * to use system GL libs remove the provided opengl32.dll

Environment variables:

- LANG: to override the default language you can set LANG=en/fr, on windows you can edit persalys.vbs
- PERSALYS_HTML_DOCUMENTATION_PATH: override path to the html documentation
- PERSALYS_NO_GL: if defined, this disables OpenGL (used for ParaView widgets)
- PERSALYS_CALIBRATION_ENGINE: if defined to "adao" and adao support is enabled, this switches to adao for calibration computations

Development
-----------
Dependencies:

- CMake >=3.8
- OpenTURNS >=1.15
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

