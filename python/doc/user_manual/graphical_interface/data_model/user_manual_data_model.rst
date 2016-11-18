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

The right click on the item proposes to modify the model, analyze the data or perform an inference.

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
    - The title of the graph, expected to be a string;
    - The plotted variable, by default the first variable.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
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
    - The title of the graph, expected to be a string;
    - The plotted variable, by default the first variable.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
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
    - The title of the graph, expected to be a string;
    - The variable plotted on the X-axis, by default the first input;
    - The variable plotted on the Y-axis, by default the first output.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
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
    - The title of the graph, expected to be a string;
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
    - The title of the graph, expected to be a string;
    - The variable plotted on the columns, by default all the input variables;
    - The variable plotted on the rows, by default all the output variables.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

3- Inference analysis
=====================

The inference analysis allows to perform a Kolmogorov goodness-of-fit test for 1-d
continuous distributions.

3-1 Definition
~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_model/inference_wizard.png
    :align: center

The user needs to choose the variables of interest of the design of experiment by checking
the corresponding lines in the table at the left side of the window. By default, all of them
are selected. To validate the window, at least one variable must be selected.

The table at the right side of the window is used to list the distributions
to test for each variable. By default, only the normal distribution is tested.
The list of distributions can be different for each variable.

To choose the distributions to test for each variable:

- In the table at the left side of the window, select the variable.
- To add a distribution to test, click on the combobox **Add** and choose a distribution.
  Then, the distribution appears in the table at the right side of the window and
  disappears from the combobox.
  The last item **All** of the combobox allows to add at the same time all the
  distributions in the table.
- To remove a distribution, select it in the table and click on **Remove**.
  Several distributions can be selected by pressing the Ctrl key or the Shift key.

At the bottom of the page, the user can specify the value of the level. The level is
the value such that :math:`\alpha = 1 - {\rm level}` is the risk of
committing a Type I error, that is an incorrect rejection of a true
null hypothesis. Its value is a float in the range :math:`]0, 1[` and is by default 0.95.

3-2 Results
~~~~~~~~~~~

The results window gathers 2 tabs for each variable:

- The tab **Summary** includes, for a selected variable, a table of all the tested
  distributions and the associated p-value. The last column
  indicates whether the distribution is accepted or not according to the given level.
  The distributions are sorted in decreasing order of p-values.

.. image:: /user_manual/graphical_interface/data_model/inference_resultWindow_tab_summary.png
    :align: center

- The tab **Distributions** includes, for a selected variable:

  - At the top, a combobox to choose the distribution to visualize and for the selected distribution:
  - The tab **Description** presents the PDF/CDF (default: PDF) of the sample
    together with the distribution PDF. It includes at its right side a table with the values
    of the distribution parameters.

  .. image:: /user_manual/graphical_interface/data_model/inference_resultWindow_tab_distributions_description.png
      :align: center

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_model/inference_resultWindow_PDF_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  - The tab **Q-Q plot** presents the Q-Q plot which opposes the data quantiles to the quantiles
    of the tested distribution.

  .. image:: /user_manual/graphical_interface/data_model/inference_resultWindow_tab_distributions_qqplot.png
      :align: center

  The user can configure the Q-Q plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/data_model/inference_resultWindow_qqplot_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.