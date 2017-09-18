=================================
User manual - Define a data model
=================================

The OTGui allows the user to load data samples and analyse them. They are considered as data models.

1- Creation
===========

A data model can be created through:
  - the context menu of the study item;
  - the button **Data model** of the study window.

The creation of a data model adds a new element in the study tree below an item named **Data models**.

The new item context menu has two actions:
  - **Define the model**: Open a new window to define the model
  - **Remove**: Remove the model and all the analyses depending on it from the study

.. image:: /user_manual/graphical_interface/physical_model/physicalModelNameContextMenu.png
    :align: center

It is associated with a 'Data model diagram' window containing an **Use pattern**.

.. image:: /user_manual/graphical_interface/data_model/dataModelDiagram.png
    :align: center

This diagram displays the possible actions that the user can perform in real time.
An action is active when its box is in dark green and disabled when its box is in light green.
A box is active when the previous one is valid.
When passing the cursor on each box, an information message appears at the bottom of the window
in order to specify what sort of actions the box proposes. If the box is disabled the message
indicates why the previous one is not valid.

On the above screenshot, the cursor has been on the **Data analysis** box.
The text explains that this box enables to analyse the sample of the model,
but this action is available only if the user has defined at least a variable in the
data model.
So, here the only option of the user is to complete the model.

2- Definition
=============

New data model can be defined through:
  - the context menu of the data model item;
  - the **Model definition** box of the model diagram.

When the definition is requiring the following window appears:

.. image:: /user_manual/graphical_interface/data_model/data_model_definition.png
    :align: center

When clicking on the **...** button, a dialog box offers to select
a file (.txt or .csv). Validate the dialog box, loads the file and its content is displayed in
the **Sample** part of the window.

Each column of this table corresponds to a variable and its name can be edited.
If the file does not defined columns names, default names are used (data_0, data_1, ...).
The line titled **Type** defines the type of each variable. By default, all of them are
input variables except the last one which is considered as an output variable.
The user can ignore columns by selecting **Disable** in the combobox.
The model must contain at least one input to validate the model.
