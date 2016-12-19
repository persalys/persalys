=================================
User manual - Define a data model
=================================

The OTGui allows the user to load data samples and analyse them. They are considered as data models.

1- Definition
=============

To define a data model, choose **New data model** in the context menu
(right click) of the OTStudy or click on the button **New data model** of the
window associated with the OTStudy.

Click on the **Import** button of the window which appears. A dialog box offers to select
a file (.txt or .csv). The file is then loaded and its content is displayed in
the **File preview** part of the window.

.. image:: /user_manual/graphical_interface/data_model/data_model_definition.png
    :align: center

Each column of this table corresponds to a variable and its name can be edited.
If the file does not defined columns names, default names are used (data_0, data_1, ...).
The line titled **Type** defines the type of each variable. By default, all of them are
input variables except the last one which is considered as an output variable.
The user can ignore columns by selecting **Disable** in the combobox.
The model must contain at least one input to validate the window.

The item created in the study tree is associated with a window summarizing the previous information.

.. image:: /user_manual/graphical_interface/data_model/data_model_display.png
    :align: center

The right click on the item proposes to modify the model, analyze the data, perform an inference or
create a meta model.