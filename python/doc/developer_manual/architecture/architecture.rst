Architecture considerations
===========================

Dependencies
------------

Several dependencies are needed in order to build the GUI:

- CMake >=3.8
- OpenTURNS >=1.19
- `otfmi <https://github.com/openturns/otfmi>`_ >=0.14 (optional, for FMI support)
- `otmorris <https://github.com/openturns/otmorris>`_ >=0.9 (optional, for Morris support)
- Qt >=5
- Qwt >=6
- Python >=3.5
- Boost.locale/filesystem
- SalomeYACS (optional, for YACS support)
- Sphinx >=1.1 (optional, for doc)
- Numpydoc (optional, for doc)
- ParaView >=5.8 (optional, for visualization)


Compilation
-----------

.. code::

    cd persalys
    mkdir -p build && cd build
    cmake \
      -DCMAKE_INSTALL_PREFIX=$PWD/install \
      -DOpenTURNS_DIR=$PWD/../../openturns/build/install/lib/cmake/openturns \
      -DSalomeKERNEL_DIR=$ROOT_SALOME_MECA/V2015_1/modules/KERNEL_V7_5_1/salome_adm/cmake_files/ \
      -DSalomeGUI_DIR=$ROOT_SALOME_MECA/V2015_1/modules/GUI_V7_5_1/adm_local/cmake_files \
      -DSalomeYACS_DIR=$ROOT_SALOME_MECA/V2015_1/modules/YACS_V7_5_1/adm/cmake/ ..

Source code structure
---------------------

.. image:: dependencies.png
    :align: center

The GUI classes are organized by 3 layers: I0, I1, V0.
This layered organization is reflected in the sources with three different folders and their associated sub-libraries.

Here is the global class diagram for each layer:

.. image:: class_diagram.png
    :align: center
