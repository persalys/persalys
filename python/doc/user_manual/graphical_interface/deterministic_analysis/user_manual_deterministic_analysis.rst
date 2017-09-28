====================================
User manual - Deterministic analyses
====================================

The OTGui allows the user to launch two types of deterministic analyses:

- Deterministic studies for defined model evaluations;
- Designs of experiments for model evaluations on a set of input values.

.. _deterministicStudy:

1- Deterministic study
===============================================

Deterministic studies consist in evaluations of the physical model for defined
input values.

New model evaluations can be created through:

- the context menu of the **Definition** item of the relevant physical model in the study tree;
- the **Model evaluation** box of the model diagram
- the context menu of the **Evaluation** item (if it already exists).

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluation.png
    :align: center

In the resulting window, the user has to define the value of each input variable
for which the evaluation has to be performed. The values are expected
to be floating points or integer. The default values are the ones defined in the physical
model window.

The user can choose the outputs to evaluate by clicking on the button **-- Select outputs --**
at the top of the window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

By default all the output variables are evaluated.

When validating this window, a new element appears in the study tree below an item
**Evaluation**.

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluationItem.png
    :align: center

Its context menu has two actions:
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analysisContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/deterministic_analysis/evaluationWindow.png
    :align: center

Click on the **Start** button launches the evaluation.

When the evaluation is finished, a result window appears. It gathers the coordinates of
the evaluation point and the corresponding output values.

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluationResults.png
    :align: center


2- Designs of experiments
=========================
Designs of experiments consist in evaluations of the physical model for a defined
set of input values.

New design of experiments can be created through:

- the context menu of the relevant model in the study tree (item **Definition** and
  item **Probabilistic model** if it exists);
- the **Design of experiments creation** box of the model diagram
- the context menu of the **Designs of experiments** item (if it already exists).

In the resulting window, the user has to choose between three types of design prior to
click on the **Continue** button:

- Deterministic
- Probabilistic
- Import data

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperiment.png
    :align: center

If the model does not contain stochastic input variables the **Probabilistic** item is disabled.

2-1 Deterministic design
''''''''''''''''''''''''
.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentDeterministic.png
    :align: center

To define a deterministic design, the user has to choose the input variables which
will vary by selecting them with the corresponding check box (default: all variables
are unselected).

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

2-2 Probabilistic design
''''''''''''''''''''''''
Define a probabilistic model to make this option available.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentProba.png
    :align: center

Three types of designs are available:

- LHS (Latin Hyper Cube)
- Monte Carlo
- Quasi-Monte Carlo

If input variables are correlated, only the Monte Carlo design is available.

The user may set:

- the size of the design of experiments;
- the seed the random generator (default: 0, positive integer expected).

2-3 Import data
'''''''''''''''

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentImport.png
    :align: center

The user may also use an external file to import a design of experiments.
The user has to set the link between the columns of the imported design and
the input variables of the physical model by clicking on the column name
(default : first column for the first input
variable, second column for the second input variable, etc...)

2-4 Design of experiments input sample
''''''''''''''''''''''''''''''''''''''

When validating the previous window, a new item is added in the study tree below an item named
**Designs of experiments**. This item is associated with a window where the input sample of
the design of experiments appears in a table.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTableX.png
    :align: center

The context menu of this new item proposes to modify the design of experiments, to evaluate it,
or to remove it.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentContextMenu.png
    :align: center

2-4 Design of experiments evaluation
''''''''''''''''''''''''''''''''''''

Definition
~~~~~~~~~~

New design of experiments can be evaluated through:

- the context menu of the relevant design in the study tree;
- the **Design of experiments evaluation** box of the model diagram

When requiring an evaluation a window appears to set:

- the interest output variables (at least one of the listed outputs to validate the window);
- the block size (i.e. number of runs launched at the same time), for parallelization purpose
  (default: 1; integer expected). It must be inferior to the input sample size.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentEvaluationWizard.png
    :align: center


When clicking on the box of the model diagram, if the physical model contains several
not evaluated design of experiments, this window lists all of them in the combo box of the
**Design of experiments** section.

When validating this window, a new item named **Evaluation** is added in the study tree below
the design of experiments item.

Its context menu has two actions:
  - **Modify**: Reopen the setting window to change the evaluation parameters;
  - **Metamodel**: Create a new metamodel.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentEvaluationContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/deterministic_analysis/analysisWindow.png
    :align: center

Click on **Start** launches the model evaluation on all the points defined in the
design of experiments. The user can stop the evaluation by clicking on the **Stop** button.

Result
~~~~~~

When the analysis is finished or stopped, the following window appears.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentMinMax.png
    :align: center

Tabs present the design of experiments and the analysis of the outputs.
The **Table** tab, the **Cobweb plot** tab, the **Plot matrix** tab and the **Scatter plots** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

- The **Min/Max** tab summarizes the result of the design. The minimum and maximum values
  of the selected output (at the left of the window) are presented with the input values which has
  led to these extrema.

- The **Table** tab displays the input and output samples.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTable.png
      :align: center

  This tab can also display other results:

  - If the some points have failed, three tabs are added:

    - The **Failed points** tab containing a table with all the failed points;

    - The **Cobweb plot** tab displaying all the points of the design of experiments.
      Its last column named **Status**, says if the point has failed (status=0) or not (status=1).

      .. image:: /user_manual/graphical_interface/deterministic_analysis/failedPointsCobweb.png
          :align: center

      The user can select the variables to show and the order of the axes with the graphic settings
      below the study tree.
      The **Export** button opens a dialog box for the user to select where the
      figure has to be saved.

    - The **Scatter plots** tab presents the scatter plot of two parameters
      (Default: first input vs second input). The succeeded points are in green and the failed ones in red.

      .. image:: /user_manual/graphical_interface/deterministic_analysis/failedPointsScatter.png
          :align: center

      The user can configure the scatter plot with the graphic settings below the study tree.

      .. image:: /user_manual/graphical_interface/deterministic_analysis/scatterGraphSetting.png
          :align: center

      The user can define:
        - The title of the graph, expected to be a string;
        - The variable plotted on the X-axis, by default the first input;
        - The variable plotted on the Y-axis, by default the second input.

      The scatter plot is plotted in the ranks space when the user checks **Ranks**.

      It has two tabs, **X-axis** and **Y-axis** where the user can define:
        - The axis title, expected to be a string;
        - The minimum bound of the axis, expected to be a floating point;
        - The maximum bound of the axis, expected to be a floating point;
        - A log scale (the chackbutton is available only if the values of the axis are positive).
      The **Plot style** tab enables to define:
        - the plot color;
        - the marker style (Cross, Circle, Diamond, Square, Plus);
        - the marker size.

      The **Export** button opens a dialog box for the user to select where the
      figure has to be saved.

  - If the some points have not been evaluated (the user stopped the analysis), two tabs are added:

    - The **Not evaluated points** tab containing a table with all the points which have not been evaluated;

    - The **Scatter plots** tab presents the scatter plot of two parameters
      (Default: first input vs second input). The succeeded points are in green and the not evaluated ones in blue.

      .. image:: /user_manual/graphical_interface/deterministic_analysis/notEvaluatedPointsScatter.png
          :align: center

      The user can configure the scatter plot with the graphic settings below the study tree.

- The **Cobweb plot** tab displays the Cobweb plot containing the input and output variables.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentCobweb.png
      :align: center

  The user can select the variables to show and the order of the axes with the graphic settings
  below the study tree.
  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Plot matrix** tab displays the plot matrix which gathers:
    - Out of the diagonal: the scatter plot of each couple of variables (inputs and outputs);
    - On the diagonal: the histogram of the distribution of each variable.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentPlotMatrix.png
      :align: center

  The user can select the variables to show and the order of the lines with the graphic settings
  below the study tree.
  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Scatter plots** tab presents the scatter plot of two parameters (Default: first output vs first input).

    .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentScatter.png
        :align: center

    The user can configure the scatter plot with the graphic settings below the study tree.

    The user can define:
      - The title of the graph, expected to be a string;
      - The variable plotted on the X-axis, by default the first input;
      - The variable plotted on the Y-axis, by default the first output.

    The scatter plot is plotted in the ranks space when the user checks **Ranks**.

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point;
      - A log scale (the chackbutton is available only if the values of the axis are positive).
    The **Plot style** tab enables to define:
      - the plot color;
      - the marker style (Cross, Circle, Diamond, Square, Plus);
      - the marker size.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

- The **Parameters** tab lists the analysis parameters.

    .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentParameters.png
        :align: center

