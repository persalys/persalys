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

The right click on the item proposes to modify the model or analyse the data.

2- Data analysis
================

The results window gathers 6 tabs:

- The tab **Summary** includes, for a selected variable:
    - The number of values;
    - The minimum and maximum values;
    - If the variable is an output, the coordinates of the input values which have generated
      the minimum and maximum values;
    - Statistics of the variable distribution:
        - Mean;
        - Standard deviation;
        - Coefficient of variation
        - Skewness;
        - Kurtosis;
        - First quartile;
        - Third quartile;
    - The probability (resp. quantile) can be defined to compute the corresponding
      quantile (resp. probability) of the sample distribution.

.. image:: /user_manual/graphical_interface/data_model/data_model_analysis_summary.png
    :align: center

- The tab **PDF/CDF** presents the PDF/CDF (default: PDF) of the sample
  together with a kernel smoothing representation.

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_PDF_setting.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted variable, by default the first variable.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_PDF.png
      :align: center

- The tab **Box plots** presents the box plot of the sample.

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_boxplot.png
      :align: center

  The user can configure the box plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_boxplot_setting.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted variable, by default the first variable.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The tab **Scatter plots** presents the scatter plot of two parameters (Default:
  first output vs first input).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_scatterplot.png
      :align: center

  The user can configure the scatter plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_scatterplot_setting.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The variable plotted on the X-axis, by default the first input;
    - The variable plotted on the Y-axis, by default the first output.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The tab **Plot matrix X-X** gathers:
    - Out of the diagonal the scatter plot of each couple of inputs;
    - The histogram of the distribution of each input on the diagonal.

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_plotmatrixXX.png
      :align: center

  The user can configure the matrix plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_plotmatrixYX_setting.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The variable plotted on the columns, by default all the input variables;
    - The variable plotted on the rows, by default all the output variables.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The tab **Plot matrix Y-X** presents the scatter plot of each output with respect
  to each input in a matrix plot. 

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_plotmatrixYX.png
      :align: center

  The user can configure the matrix plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/data_model_analysis_plotmatrixYX_setting.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The variable plotted on the columns, by default all the input variables;
    - The variable plotted on the rows, by default all the output variables.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

