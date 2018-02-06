====================================
User manual - Deterministic analyses
====================================

The OTGui allows the user to launch three types of deterministic analyses:

- Model evaluations;
- Designs of experiments for model evaluations on a set of input values;
- Screening.

.. _deterministicStudy:

1- Model evaluation
===================

The user may evaluate the physical model for defined input values.

1-1 Definition
''''''''''''''

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

3-2 Results
'''''''''''

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

The user may defined a set of input values and then evaluate output variable at these points.

2-1 Design of experiments creation
''''''''''''''''''''''''''''''''''

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

2-1-1 Deterministic design
~~~~~~~~~~~~~~~~~~~~~~~~~~
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

2-1-2 Probabilistic design
~~~~~~~~~~~~~~~~~~~~~~~~~~
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

2-1-3 Import data
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentImport.png
    :align: center

The user may also use an external file to import a design of experiments.
The user has to set the link between the columns of the imported design and
the input variables of the physical model by clicking on the column name
(default : first column for the first input
variable, second column for the second input variable, etc...)

2-1-4 Design of experiments input sample
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When validating the previous window, a new item is added in the study tree below an item named
**Designs of experiments**. This item is associated with a window where the input sample of
the design of experiments appears in a table.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTableX.png
    :align: center

The context menu of this new item proposes to modify the design of experiments, to evaluate it,
or to remove it.

.. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentContextMenu.png
    :align: center

2-2 Design of experiments evaluation
''''''''''''''''''''''''''''''''''''

2-2-1 Definition
~~~~~~~~~~~~~~~~

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

2-2-2 Results
~~~~~~~~~~~~~

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
        - A log scale (the checkbutton is available only if the values of the axis are positive).
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
      - A log scale (the checkbutton is available only if the values of the axis are positive).
    The **Plot style** tab enables to define:
      - the plot color;
      - the marker style (Cross, Circle, Diamond, Square, Plus);
      - the marker size.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

- The **Parameters** tab lists the analysis parameters.

    .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentParameters.png
        :align: center


3- Screening
===================

OTGui proposes one method of screening, the `Morris method <http://openturns.github.io/otmorris/master/index.html>`_.

3-1 Definition
''''''''''''''

New screening analysis can be created thanks to:
  - the context menu of the **Definition** item of the relevant physical model in the study tree;
  - the **Screening** box of the physical model diagram.

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisWizard_FirstPage.png
    :align: center

One method is available:
  - Morris.

The user can choose the outputs to analyse by clicking on the button **-- Select Outputs --**
at the top of the window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

By default all the output variables are analysed.

On the second page, the user has to define the lower and upper bounds for each variable.
The values of the variables will be regularly spaced between these bounds.

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisWizard_secondPage.png
    :align: center

In the section **Parameters**, on the third page, the user can define:

- The number of trajectories (default: 10; integer expected)
- The level (default: 5; integer expected) to define a regular grid.

In the advanced parameters (default: hidden), the user can define:

- The seed of the random generator (default: 0, positive integer expected).

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisWizard_thirdPage.png
    :align: center


3-2 Results
''''''''''''''

When validating the previous window, a new element appears in the study tree below an item
named **Sensitivity**.

Its context menu has two actions:
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisWindow.png
    :align: center

Click on the **Start** button launches the analysis. The user can stop the analysis by clicking
on the **Stop** button.

When the analysis is finished or stopped, a result window appears.

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisResultWindow.png
    :align: center

At the left of the result window the section **Outputs** enables the user to choose the result to display.

The results window gathers 6 tabs:

- The tab **Elementary effects** presents for a selected output:
  a table with the effects of each input variable and the mean of the absolute value (:math:`\mu*`),
  the standard deviation (:math:`\sigma`) and the mean (:math:`\mu`) of the elementary effects.
  These values are plotted in the graphs :math:`(\mu*, \sigma)` and :math:`(\mu*, \mu)`.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/morrisGraph.png
      :align: center

  The types of effects are determined automatically according to two criteria
  modeled by two lines in the graph :math:`(\mu*, \sigma)`:

  - the green vertical dotted line, called 'No effect boundary' is defined by:

    :math:`\mu_b* = \frac{max(\mu_i*) - min(\mu_i*)}{2}`, (with :math:`i` in :math:`[0, N]` and :math:`N` the number of inputs).
    It separates the variables into two groups:

    - the variables which have not an effect (:math:`\mu_i < \mu_b*`)
    - the variables which have an effect (:math:`\mu_i < \mu_b*`)

    This boundary :math:`\mu_b*` can be changed by the user by clicking (left-click) on the X-axis.

  - the blue dotted line, called 'cv = 0.5' is defined by: :math:`cv = \frac{\sigma_i}{\mu_i*} = 0.5`.
    It separates the variables into two groups:

    - the variables which have a linear effect (:math:`\frac{\sigma_i}{\mu_i*} <= 0.5`)
    - the variables which have a non-linear effect or interaction (:math:`\frac{\sigma_i}{\mu_i*} > 0.5`)

  - On the graph :math:`(\mu*, \mu)`, the blue lines separate the variables into two groups:

    - the monotonic variables (:math:`|\mu_i| \simeq \mu_i*`)
    - the non-monotonic variables (:math:`|\mu_i| < \mu_i*`)

    The sign of :math:`\mu_i` gives the direction of the effect.

  By default, in the table, the variables with no effect are deselected.
  The user can de/select variables manually:

  - by changing the state of the checkbox associated with the variable in the table
  - by de/selecting points in the graphs (select point with a right-click, then click on de/select
    button of the context menu which appears).

  .. image:: /user_manual/graphical_interface/deterministic_analysis/morrisGraph_selection.png
      :align: center

  The selection of the influential variables is saved. This result can be used in
  the probabilistic model definition window to transform the non influential
  stochastic variables into deterministic variables.

- The **Table** tab presents the sample generated by the Morris method and the resulting output values.

- The **Cobweb plot** tab displays the Cobweb plot containing the input and output variables.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/morrisCobweb.png
      :align: center

  The user can select the variables to show and the order of the axes with the graphic settings
  below the study tree. The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Plot matrix** tab displays the plot matrix which gathers:
    - Out of the diagonal: the scatter plot of each couple of variables (inputs and outputs);
    - On the diagonal: the histogram of the distribution of each variable.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/morrisPlotMatrix.png
      :align: center

  The user can select the variables to show and the order of the lines with the graphic settings
  below the study tree. The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Scatter plots** tab presents the scatter plot of two parameters (Default: first output vs first input).

    .. image:: /user_manual/graphical_interface/deterministic_analysis/morrisScatter.png
        :align: center

    The user can configure the scatter plot with the graphic settings below the study tree.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyScatterConfig.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;
      - The variable plotted on the X-axis, by default the first input;
      - The variable plotted on the Y-axis, by default the first output.

    The scatter plot is plotted in the ranks space when the user checks **Ranks**.

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point;
      - A log scale (the checkbutton is available only if the values of the axis are positive).
    The **Plot style** tab enables to define:
      - the plot color;
      - the marker style (Cross, Circle, Diamond, Square, Plus);
      - the marker size.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

3-3 Use results
''''''''''''''''''

In the :ref:`probabilistic model <probaModel>` definition window, click on the button **Import Morris result**
placed below the table.

.. image:: /user_manual/graphical_interface/deterministic_analysis/probaModelBefore.png
    :align: center

The followed window appears (only if a result exists):

.. image:: /user_manual/graphical_interface/deterministic_analysis/morrisResultWizard.png
    :align: center

Choose the suitable result and click on the **Finish** button.
The probabilistic model is modified as follows:

.. image:: /user_manual/graphical_interface/deterministic_analysis/probaModelAfter.png
    :align: center

Of course loading a result while all the input variables are already deterministic will not work...
