=========================
User manual - First steps
=========================

0- Configuration
================

- To override the default language, it is advised to proceed as follows:

  - Linux: you must launch the appImage with the LANGUAGE environment
    variable LANGUAGE set to en/fr as follows:

  .. code::

     LANGUAGE=en ./persalys-x.x.x-x86_64.AppImage

  - Windows: you must add an argument to the launcher. Modify the
    target link of the shortcut in the start menu and/or in the
    desktop and add the argument ``--lang en`` or ``--lang fr``.  You
    should have this if Persalys is installed in user mode, the target
    will be ``C:\Users\<user_name>\AppData\Local\Persalys\Lib\site-packages\persalys\persalys_launcher.exe --lang en``

- Persalys Python environment can be amended with additional packages
  by using the included nopip module. Additional modules can be
  installed for standalone versions only (installed Windows installer
  or Linux AppImage) by typing in the following in Persalys :ref:`Python
  console <pythonconsole>`:

  .. code::

     from nopip import install
     install.modules(["<module_name>", "--user"], verbose=True)


  where ``<module_name>`` is the module name provided to the usual pip
  install command. Modules are installed in ``~/.persalys_base`` on
  Linux and in ``<USER_DIR>\AppData\Roaming\Persalys\`` on Windows


1- Getting started with the graphical interface
===============================================

1-1 Main window
'''''''''''''''
.. image:: /user_manual/graphical_interface/getting_started/generalOverview.png
    :align: center
    :width: 640px

The main window of the graphical interface consists of a working space in which the other
windows (model definition or result window) open, surrounded by:

- The **study tree** displays the models and analyses performed in the studies. Elements are organized
  in sections, in bold (ex: **Physical models**, **Evaluation**, **Definition**), in which the user can create analyses and models.

  .. image:: /user_manual/graphical_interface/getting_started/tree.png
      :align: center


- The main **menu bar** and **tool bar**.

  .. image:: /user_manual/graphical_interface/getting_started/menuBar.png
      :align: center

.. _pythonconsole:

- The **Python console** shown by default, can be used to type in and
  run Python commands within Persalys environment.

  .. image:: /user_manual/graphical_interface/getting_started/pythonConsole.png
      :align: center


- The **Graph settings** window appears when a figure is displayed in the main window to set up graphical parameters.
  Its content varies according to the type of graph:

.. _firstgraphsettings:

    - for the **Parallel coordinates plots** and the **Plot matrix**, the window allows one to:
       - Select the variables to display and the axis order
       - Modify the space where the values are plotted with the **Ranks** check button
         (default: physical space). Check off the button to display the rank of the values
       - Export the figure

       .. image:: /user_manual/graphical_interface/getting_started/cobweb_plotMatrix_Graph_settings.png
         :align: center


.. _secondgraphsettings:

    - for the charts, the window allows one to:
       - Export the figure
       - Specify the graph title (expected: string)
       - Specify axis title (expected: string) in the **X-axis** and **Y-axis** tabs
       - Depending on the graph:
           - Select data to plot
           - Modify the space where the values are plotted with the **Ranks** check button
             (default: physical space). Check off the button to display the rank of the values
           - Specify other axis parameters in the **X-axis** and **Y-axis** tabs:
               - minimum/maximum bounds (expected: floating point)
               - log scale (check button only available if axis values are positive)
               - X-axis label orientation
           - Reset the axis ranges
           - Specify the **plot style**: color, marker size and style (cross, circle, diamond, square, plus)

      .. image:: /user_manual/graphical_interface/getting_started/graphicConfiguration.png
        :align: center


1-2 On startup
''''''''''''''

.. image:: /user_manual/graphical_interface/getting_started/window_startUp.png
    :align: center

The interface startup window shows 3 items:
  - **New study**: Create a new study.
  - **Open study**: Choose an existing study from a dialog box.
    The loaded study becomes the current study.
  - **Import Python script**: Load a Python Script from a dialog box.
    It is impossible to load a study when another one is already opened.


.. |infoButton| image:: /user_manual/graphical_interface/probabilistic_analysis/documentinfo.png

- Click on the icon |infoButton| to open the documentation

1-3 Menus
'''''''''

File Menu
~~~~~~~~~

.. image:: /user_manual/graphical_interface/getting_started/fileMenu.png
    :align: center

The **File** menu gathers the following shortcuts:

- **New**: Create a new study.

- **Open...**: Choose an existing study from a dialog box.
  The loaded study becomes the current study.

- **Open Recent**: Quick access to recently loaded studies.

- **Save**: Save the current study.

- **Save As...**: Choose the directory where to save the current project.

- **Import Python...**: Load a Python Script from a dialog box.

- **Close**: Close the current Study. A pop-up dialog window appears if the current Study has not been saved.
  Any Study may also be closed through its context menu.

- **Exit**: Quit the interface.

Tools Menu
~~~~~~~~~~

.. image:: /user_manual/graphical_interface/getting_started/viewMenu.png
    :align: center

The **Tools** menu allows the user to :

- display/hide the Python console
- set the default number of processes for Python/Coupling/FMI physical models

Help Menu
~~~~~~~~~

.. image:: /user_manual/graphical_interface/getting_started/helpMenu.png
    :align: center

The **Help** menu gathers the following shortcuts:

- **User's manual**: Open the user's manual in a Web browser.

- **About Persalys**: Display information (version, license, libraries, etc.) about the interface in a pop-up dialog window.

2- Create a study
=================

.. |newButton| image:: /user_manual/graphical_interface/getting_started/document-new22x22.png


There are several ways to create a new study:

- Use the button **New Study** of the startup window

  .. image:: /user_manual/graphical_interface/getting_started/startUpWindowButtons.png
      :align: center

- Click on |newButton| in the tool bar

- Click on **New** in the file menu

- Press Ctrl+N

When a study is created, an item appears in the study tree. This item is associated with a window
which provides ways to define models:

.. image:: /user_manual/graphical_interface/getting_started/window_OTStudy_startUp.png
    :align: center

The context menu of study item gathers the same actions as the window:

  .. image:: /user_manual/graphical_interface/getting_started/studyItemContextMenu.png
      :align: center

.. _modelsdefonstudywindow:

It proposes to create models:
  - Models defined with a vector function:
     - **Symbolic model**: Physical model defined with analytical formulas
     - **Python model**: Physical model defined with a Python script
     - **YACS model**: Physical model defined with a YACS scheme (optional)
     - **FMI model**: Physical model defined with a FMU file (optional)
  - Models defined with a field function:
     - **Symbolic Field model**: Physical model defined with analytical formulas and a mesh
     - **Python Field model**: Physical model defined with a Python script and a mesh
  - Models defined with a data sample:
     - **Data model**: Physical model defined with a sample

The context menu also allows the following actions:
  - **Rename**: Rename the study
  - **Export Python**: Choose a directory to export the study as a Python script
  - **Save**: Save the study
  - **Close**: Close the study. A pop-up dialog window appears if the study has not been saved
