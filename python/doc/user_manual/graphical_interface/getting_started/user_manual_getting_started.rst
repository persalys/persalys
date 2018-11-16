=========================
User manual - First steps
=========================

1- Getting started with the graphical interface
===============================================

1-1 Main window
'''''''''''''''
.. image:: /user_manual/graphical_interface/getting_started/generalOverview.png
    :align: center
    :width: 640px

The main window of the graphical interface consists of a working space in which the other
windows (model definition or result window) open, surrounded by:

- The study tree (on the left),

  .. image:: /user_manual/graphical_interface/getting_started/tree.png
      :align: center

  It contains the study and all subsequent models defined and analysis performed.
  The name of each element created by can be modified by the user by double-click on the element.
  The elements in bold (such as **Physical models**, **Evaluation**, **Definition**, etc)
  cannot be renamed.
  The right click on these elements gives access to the context menu of the elements. Any element
  can be removed through this context menu.

- The main menubar and toolbar (on top).

  .. image:: /user_manual/graphical_interface/getting_started/menuBar.png
      :align: center


- The Python console (at the bottom) shown by default,

  .. image:: /user_manual/graphical_interface/getting_started/pythonConsole.png
      :align: center


- The graphic settings (below the study tree) which is shown when figures are displayed.

  .. image:: /user_manual/graphical_interface/getting_started/graphicConfiguration.png
      :align: center


1-2 On start up
'''''''''''''''

.. image:: /user_manual/graphical_interface/getting_started/window_startUp.png
    :align: center

A window appears when the OTGui is started. It contains 3 items:
  - **New study**: Create a new Study
  - **Open study**: Display a dialog box for choosing a study
    to load. The loaded study will become the current
    study.
  - **Import Python script**: Display a dialog box for loading an OTGui Python script.
    It is impossible to load a study when another one is already opened.


1-3 Menus
'''''''''

File Menu
~~~~~~~~~

.. image:: /user_manual/graphical_interface/getting_started/fileMenu.png
    :align: center

The file menu gathers the following shortcuts:

- **New**: Create a new study

- **Open...**: Display a dialog box for choosing a study
  to load. The loaded study will become the current
  study.

- **Open Recent**: Display a list of studies recently used for opening quickly one of them

- **Save**: Save the current study

- **Save As...**: Display a dialog box for choosing the directory to save the current project.

- **Import Python...**: Display a dialog box for loading an OTGui Python script.

- **Close**: Close the current Study. A pop-up dialog window will appear if the current Study has not been saved.
  Any Study may also be closed through its context menu.

- **Exit**: Quit OTGui

View Menu
~~~~~~~~~

.. image:: /user_manual/graphical_interface/getting_started/viewMenu.png
    :align: center

The view menu allows the user to display/hide the Python console.

2- Create a study
=================

.. |newButton| image:: /user_manual/graphical_interface/getting_started/document-new22x22.png

There are three ways to create a new study:

- Use the button **New Study** of the window which appears when the OTGui is launched;

- Click on |newButton| in the tool bar;

- Click on **New** in the file menu.

When a study is created, an item appears in the study tree. This item is associated with a window
which provides ways to define models:

.. image:: /user_manual/graphical_interface/getting_started/window_OTStudy_startUp.png
    :align: center

The context menu of study item gathers the same actions as the window:
  - **Symbolic model**: Define a physical model with analytical formulas
  - **Python model**: Define a physical model with a Python script
  - **Data model**: Define a physical model with samples

and the following actions:
  - **Rename**: Rename the study;
  - **Export Python**: Display a dialog box for choosing the directory to export the study as an OTGui Python script;
  - **Save**: Save the study;
  - **Close**: Close the study. A pop-up dialog window will appear if the study has not been saved.
