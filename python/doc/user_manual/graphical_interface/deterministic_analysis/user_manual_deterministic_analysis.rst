====================================
User manual - Deterministic analyses
====================================

The OTGui allows the user to launch two types of deterministic analyses:

- Deterministic studies for defined model evaluations;
- Designs of experiment for model evaluations on a set of input values.

.. _deterministicStudy:

1- Deterministic study
===============================================

Deterministic studies consist in evaluations of the physical model for defined
input values.

New model evaluations can be created through the context menu of the
Deterministic study in the study tree, for the relevant physical model.

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluation.png
    :align: center

In the resulting window, the user has to define the value of each input variables
for which the evaluation has to be performed. The values are expected
to be floating points or integer.

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluationResults.png
    :align: center

The evaluation is launched when using the **Done** button. The result window of
the evaluation gathers the coordinates of the evaluation point and the corresponding
output values.

An item is created in the study tree which allows the user to retrieve the result
of the deterministic study.

2- Designs of experiment
========================
Designs of experiment consist in evaluations of the physical model for a defined
set of input values.

New design of experiment can be created through the context menu of the Designs
of experiment in the study tree, for the relevant physical model.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperiment.png
    :align: center

The user has to choose between two types of design prior to click on the **Continue**
button:

- Deterministic
- Import data

2-1 Deterministic design
''''''''''''''''''''''''
.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentDeterministic.png
    :align: center

To define a deterministic design, the user has to choose the input variables which
will vary by selecting them with the corresponding check box (default: all variables
are selected).

- **Varying input variables**:
  The user has to define the lower and upper bound for the variable. The values
  of the variable will be regularly spaced between the bounds.

  If **Levels** is selected in the last column (selected by default),
  the number of intermediate values is defined by the level value. 

  If **Delta** is selected in the last column (click on the last column name),
  the space between two points is defined by the delta parameter. The bounds are always added.

- **Fixed input variables**:
  The user has to define the value of the variable. By default it is set to its
  value in the physical model definition. The values are expected to be floating points or integer.

2-2 Import data
'''''''''''''''

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentImport.png
    :align: center

The user may also use an external file to import a design of experiment.
The user has to set the link between the columns of the imported design and
the input variables of the physical model by clicking on the column name
(default : first column for the first input
variable, second column for the second input variable, etc...)

2-3 Results
'''''''''''

The design of experiment appears in a table and can be launched by using the **Evaluate** button.
The corresponding values of the outputs will be added to the table.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTable.png
    :align: center

An item is created in the study tree which allows the user to retrieve the result
of the design of experiment.

- The tab **Min/Max** summarizes the result of the design. The minimum and maximum values
  of the selected output are presented with the input values which has led to these
  extrema.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentMinMax.png
      :align: center

- The tab **Scatter plots** presents the scatter plot of two parameters (Default:
  first output vs first input).

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentScatter.png
      :align: center

  The user can configure the scatter plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentScatterConfig.png
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


- The tab **Plot matrix Y-X** presents the scatter plot of each output with respect
  to each input in a matrix plot. 

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentYX.png
      :align: center

  The user can configure the matrix plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentYXConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The variable plotted on the columns, by default all the input variables;
    - The variable plotted on the rows, by default all the output variables.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The tab **Plot matrix X-X** gathers:
  - Out of the diagonal the scatter plot of each couple of inputs;
  - The histogram of the distribution of each input on the diagonal.

      .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentXX.png
          :align: center

    The user can configure the matrix plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentYXConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The variable plotted on the columns, by default all the input variables;
    - The variable plotted on the rows, by default all the output variables.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

