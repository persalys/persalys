====================================
User manual - Probabilistic analyses
====================================

.. _probaModel:

1- Probabilistic model
======================

In order to perform probabilistic analysis, the user needs first to build a
probabilistic model.

New probabilistic model can be created through:
  - the context menu of the **Definition** item of the relevant physical model in the study tree;
  - the **Probabilistic model definition** box of the model diagram

A physical model can contain only one probabilistic model.

At the creation of a probabilistic model, a new item named **Probabilistic model** is added in the
study tree below the physical model item.

Its context menu contains four actions:
  - **Design of experiments**: Create a new design of experiments;
  - **Limit state**: Create a new limit state;
  - **Central tendency**: Create a new central tendency analysis;
  - **Sensitivity**: Create a new sensitivity analysis;

.. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelContextMenu.png
    :align: center

This item is associated with a window which has two tabs:

- the **Marginals** tab:
    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelMarginals.png
        :align: center

    The left side of the tab contains a table listing all the input variables defined in the physical model.
    By default all of them are deterministic variables.
    To make a variable stochastic the user has to check its corresponding line. By default the variable follows
    the Gaussian distribution. To change its distribution the user can use the combo box of the
    **Distribution** column.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelDistributionsList.png
        :align: center

    The right side of the probabilistic model window is updated depending of the selected variable and
    its distribution. It shows the parameters of the distribution together with truncation parameters.
    The values are expected to be floating points or integers, with respect to the distribution support.
    If a wrong value is used, it will return to its previous value and an error message will appear at the bottom
    of the window.

    .. |infoButton| image:: /user_manual/graphical_interface/probabilistic_analysis/documentinfo.png

    The icon |infoButton| is an hyperlink to the OpenTURNS documentation.

    There are different ways to set the parameters of the LogNormal, Weibull and Gumbel distributions.
    To select the good one, click on the **Type** select button.

    The user can configure the plot of the distribution and choose between PDF/CDF with the graphic
    settings below the study tree.

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

    In the proposed distributions list, there is the item **Inference result**. If the current study has at least
    an :ref:`inference analysis result <inferenceAnalysis>`, when clicking on
    this item, the following window appears:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/inference_resultWizard.png
        :align: center

    The user can look for an analysis result with the **Inference analysis** combo box.
    Then the user can choose a result of a variable with the **Variable** combo box.
    The user can choose one distribution in the displayed list. The right side of the window displays
    the current distribution parameters.
    The distributions with which the inference analysis failed ('failed' indicated in the acceptance column) are
    listed for convenience but it is not possible to validate the window when selecting one of them.

    When clicking on the **Finish** button, the chosen distribution is defined for the selected variable.

- the **Dependence** tab:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelDependence.png
        :align: center

    All the stochastic input variables are listed at the left side of this tab.
    By default no dependence is set between these variables.

    To add dependence the user has to:
      - create a group of at least two variables by selecting the variables in the list
      - add this group in the table by clicking on the right arrow
      - choose a copula in the list in the second column of the table.
        If the group contains more than two variables only the normal copula is available.

      .. image:: /user_manual/graphical_interface/probabilistic_analysis/copulasList.png
          :align: center

      - parametrize the copula at the right side of the window

    By default the dependence is modeled by a Normal copula defined by a correlation matrix equal to
    the identity matrix (so the variables are independent...)

    The user may define correlation between the variables by modifying the upper triangular
    part of the Spearman correlation matrix. OTGui handles automatically the symmetry of the correlation
    matrix. The values are expected to be floating points or integers,
    included in the interval :math:`\left[-1; 1\right]`.
    The Spearman correlation matrix must be definite positive. If a wrong value is used, it will return
    to its previous value and an error message will appear at the bottom of the window.

    Example:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelCorrelation.png
        :align: center

    To remove dependence, select a group in the table and click on the left arrow.

    The icon |infoButton| is an hyperlink to the OpenTURNS documentation.

    The user can configure the plot of the distribution and choose between PDF/CDF with the graphic
    settings below the study tree.

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

    In the copulas list, there is the item **Inference result**. If the current study has at least
    a :ref:`dependence inference result <dependenceInference>`, when clicking on
    this item, the following window appears:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/dependenceInference_resultWizard.png
        :align: center

    The user can look for an analysis result with the **Inference analysis** combo box.
    Then the user can choose a result of a group of variables with the **Group of variables** combo box.
    The user can choose one copula in the displayed list. The right side of the window displays
    the current copula PDF, Kendall plot and parameters.
    The copulas with which the inference analysis failed ('-' indicated in the
    **Bayesian Information Criterion** column) are
    listed for convenience but it is not possible to validate the window when selecting one of them.

    When clicking on the **Finish** button, the chosen copula is defined for the selected group of variables.

2- Limit state
==============

To perform a reliability analysis the user must define first a limit state.
A limit state can be defined only if the model contains stochastic input variables.

A new limit state can be created through:
  - the context menu of the probabilistic model item;
  - the **Limit state definition** box of the physical model diagram;
  - the context menu of the **Reliability** item (if it already exists).

When creating a limit state a new element is added in the study tree below the item named
**Reliability**.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateItem.png
    :align: center

Its context menu contains two actions:
  - **Threshold exceedance**: Create a new threshold exceedance analysis;
  - **Remove**: Remove the limit state and all the analyses depending on it

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateContextMenu.png
    :align: center

This item is associated with the following window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitState.png
    :align: center

In this window, the user has to define the threshold defining the
failure domain corresponding to the selected output. By default it equals 0. The values are expected
to be floating points or integers. The available operators to define the failure event are:

- <= (less or equal);
- >= (greater or equal);
- > (greater than);
- < (less than).

3- Threshold exceedance analysis
================================

The user can require a computation of the probability of failure relative to the limit state through:
  - the context menu of the relevant limit state;
  - the **Reliability** box of the physical model diagram.

3-1 Definition
''''''''''''''

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityAnalysis.png
    :align: center

The window proposes simulation methods:
  - Monte Carlo
  - FORM - Importance sampling : the FORM algorithm starts from the result of the Importance sampling analysis

and approximation methods:
  - First Order Reliability Method (FORM)
  - Second Order Reliability Method (SORM)

Simulation methods
~~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilitySimu.png
    :align: center

For the two simulation methods the user has to define the stop criteria of the algorithm.
  - The maximum coefficient of variation for the probability, by default equal to 0.01,
    expected to be a floating point or an integer, in the interval :math:`\left[0;1 \right]`;
  - The maximum computation time, by default equal to one minute
    (d means days, h means hours, m means minutes, s means seconds);
  - The maximum sampling size, by default equal to 10000 if this criterion is chosen,
    expected to be an integer.

At least one criterion must be selected to validate the window.

In the section **Evaluation parameter** the user can define:
  - The block size (i.e. number of runs launched at the same time), for parallelization purpose
    (default: 1; integer expected). It must be inferior to the maximum sampling size.

In the advanced parameters (default: hidden), the user may also set:
  - The seed of the random generator (default: 0, positive integer expected);

Approximation methods
~~~~~~~~~~~~~~~~~~~~~

For the approximation methods the user has to define optimization parameters:
  - The algorithm method: Abdo-Rackwitz, Cobyla (default), SQP
  - The starting point: by default it is the means of the distributions of the stochastic inputs

In the advanced parameters (default: hidden), the user may also set the stop criteria of the optimization:
  - The maximum number of iterations (default: 100, positive integer expected)
  - The errors: absolute, relative, residual and constraint errors (default: 1e-5, positive float expected)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityApprox.png
    :align: center

3-2 Launch
''''''''''

When validating the previous window, a new element appears in the study tree below an item
named **Reliability**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analysisContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/probabilistic_analysis/reliabilityWindow.png
    :align: center

Click on the **Start** button launches the analysis. The user can stop the analysis by clicking
on the **Stop** button.

3-3 Results
'''''''''''

When the analysis is finished or stopped, a new result window appears.

Monte Carlo
~~~~~~~~~~~

At the left of the result window the section **Output** reminds the user about the output used
for the probability estimation.
The result window has the following tabs:

- The **Summary** tab gathers:
    - The elapsed computation time;
    - The number of simulations (i.e. calls to the physical model) performed;
    - The failure probability, its coefficient of variation and the corresponding
      confidence interval at 95%.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilitySummary.png
        :align: center
    
- The **Histogram** tab presents the histogram of the output sample.

  The user can configure the histogram plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityHistogram.png
      :align: center

- The **Convergence graph** tab presents the convergence of the Monte Carlo
  algorithm.

  The user can configure the convergence plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityConvergence.png
          :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

FORM
~~~~

At the left of the result window the section **Output** reminds the user about the output used
for the probability estimation.
The result window has the following tabs:

- The **Summary** tab gathers:
    - The failure probability and the Hasofer reliability index
    - The optimization stop criteria value: 
        - The number of iterations;
        - The errors: absolute, relative, residual and constraint errors.
    - The number of calls of the model

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMSummary.png
        :align: center

- The **Design point** tab gathers:
    - The coordinates of the design point in the standard space and in the physical space;
    - The importance factors of each variable.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMDesignPoint.png
        :align: center

- The **Sensitivities** tab gathers:
    - The sensitivity of the failure probability and of the reliability index to the parameters
      of the marginals of the stochastic input variables.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMSensitivities.png
        :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

SORM
~~~~

The SORM result window contains the same tabs as the FORM result window.

However the **Summary** tab contains additional results:
  - The failure probability and the reliability index computed with the **Breitung** formula;
  - The failure probability and the reliability index computed with the **Hohen Bichler** formula;
  - The failure probability and the reliability index computed with the **Tvedt** formula.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sormResultWindow.png
    :align: center

FORM-IS
~~~~~~~

The FORM-IS result window contains the same tabs as the Monte Carlo result window and a **FORM results**
tab to display the tabs of a FORM result window.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMIS.png
    :align: center

3- Central tendency analysis
============================

New central tendency analysis can be created thanks to:
  - the context menu of the probabilistic model item;
  - the **Central tendency** box of the physical model diagram;
  - the context menu of the **Central tendency** item (if it already exists).

3-1 Definition
''''''''''''''

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendency.png
    :align: center

Two methods are available:
  - Monte Carlo sampling;
  - Taylor expansions (second order).

The user can choose the outputs to analyze by clicking on the button **-- Select Outputs --**
at the top of the window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

By default all the output variables are analyzed.

Monte Carlo
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyMC.png
    :align: center

The user has to define the stop criteria of the algorithm.
  - The maximum coefficient of variation for the mean, by default equal to 0.01,
    expected to be a floating point or an integer, in the interval :math:`\left[0;1 \right]`;
  - The maximum computation time, by default equal to one minute
    (d means days, h means hours, m means minutes, s means seconds);
  - The maximum sampling size, by default equal to 10000 if this criterion is chosen,
    expected to be an integer.

At least one criterion must be selected to validate the window.

In the section **Evaluation parameter** the user can define:
  - The block size (i.e. number of runs launched at the same time), for parallelization purpose
    (default: 1; integer expected). It must be inferior to the maximum sampling size.

In the advanced parameters (default: hidden), the user can choose:
  - To compute the confidence interval thanks to the checkbox (default: checked)
    at the given level;
  - To set the seed of the random generator (default: 0, positive integer expected).

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylor.png
    :align: center

3-2 Launch
'''''''''''

When validating the previous window, a new element appears in the study tree below an item
named **Central tendency**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analysisContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyWindow.png
    :align: center

Click on the **Start** button launches the analysis. The user can stop the analysis by clicking
on the **Stop** button.

3-3 Results
'''''''''''

When the analysis is finished or stopped, a result window appears.

Monte Carlo
~~~~~~~~~~~

The **Table** tab, the **Cobweb plot** tab, the **Plot matrix** tab and the **Scatter plots** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

At the left of the result window the section **Variables** enables the user to choose the result to display.
The results window gathers 6 tabs:

- The **Summary** tab includes, for a selected variable:
    - The elapsed computation time;
    - The number of simulations (i.e. calls to the physical model) performed;
    - The minimum and maximum variable values generated by the Monte Carlo sampling method;
    - The coordinates of the input values which have generated the minimum and maximum
      values of the selected output;
    - Statistics of the output distribution:
        - Mean and corresponding confidence interval;
        - Standard deviation and corresponding confidence interval;
        - Skewness;
        - Kurtosis;
        - First quartile;
        - Third quartile;
    - The probability (resp. quantile) can be defined to compute the corresponding
      quantile (resp. probability) of the output sample distribution.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencySummary.png
      :align: center

- The **PDF/CDF** tab presents the PDF/CDF (default: PDF) of the variable sample
  together with a kernel smoothing representation.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPDF.png
      :align: center

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings below the study tree.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPDFConfig.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Box plots** tab presents the box plot of the variable sample.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyBoxplot.png
      :align: center

  The user can configure the box plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyBoxplotConfig.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Table** tab presents the sample generated by the Monte Carlo sampling
  method and the resulting output values.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTable.png
      :align: center


- The **Cobweb plot** tab displays the Cobweb plot containing the input and output variables.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyCobweb.png
      :align: center

  The user can select the variables to show and the order of the axes with the graphic settings
  below the study tree. The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Plot matrix** tab displays the plot matrix which gathers:
    - Out of the diagonal: the scatter plot of each couple of variables (inputs and outputs);
    - On the diagonal: the histogram of the distribution of each variable.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyYX.png
      :align: center

  The user can select the variables to show and the order of the lines with the graphic settings
  below the study tree. The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Scatter plots** tab presents the scatter plot of two parameters (Default: first output vs first input).

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyScatter.png
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

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendency_MC_tab_Parameters.png
    :align: center

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylorResults.png
    :align: center

At the left of the result window the section **Outputs** enables the user to choose the result to display.

The results window gathers, for the selected output:

- The output mean corresponding to the first and second order expansions;
- The standard deviation of the output;
- The variance of the output.

4- Sensitivity analysis
=======================

New sensitivity analysis can be created thanks to:
  - the context menu of the probabilistic model item
  - the **Sensitivity** box of the physical model diagram
  - the context menu of the **Sensitivity** item (if it already exists).

The input variables must be independent to perform a sensitivity analysis.

4-1 Definition
''''''''''''''

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisMethods.png
    :align: center

Two types of sensitivity indices are available:
  - Sobol
  - SRC (Standardised Regression Coefficient)

The user can choose the outputs to analyze by clicking on the button **-- Select Outputs --**
at the top of the window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

Sobol' indices
~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSobol.png
    :align: center

The user has to define the stop criteria of the algorithm:
  - The maximum coefficient of variation for the first order indices, by default equal to 0.01,
    expected to be a floating point or an integer, in the interval :math:`\left[0;1 \right]`;
  - The maximum computation time, by default equal to one minute
    (d means days, h means hours, m means minutes, s means seconds);
  - The maximum calls, by default equal to 10000 if this criterion is chosen, expected to be an integer.

At least one criterion must be selected to validate the window.

In the section **Evaluation parameter** the user can define:
  - The block size (i.e. number of runs launched at the same time), for parallelization purpose
    (default: 1; integer expected). It must be inferior to the maximum sampling size.
    The resulting number of simulations (i.e. calls to the physical model) by iteration is given below;

In the advanced parameters (default: hidden), the user can set:
  - The Bootstrap sampling size (default: 100, positive integer expected) use
    to compute the confidence intervals.
  - The confidence level (default: 0.95; float expected).
  - The seed of the random generator (default: 0, positive integer expected).

The label number of calls by iteration is updated according to the given value
of the block size.
The algorithm generates two input samples with a size equal to the block size value
and combined these samples to build nbInputs other samples
(nbInputs is the number of input variables).

Number of calls by iteration = (nbInputs + 2) * blockSize

If the maximum calls is a stop criteria, at the last iteration we compute a value of the
block size according to not exceed the maximum calls.
See the :ref:`Sensitivity <SobolExample>` section in the example guide.

SRC indices
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSRC.png
    :align: center

In the section **Evaluation parameters** the user can define:

- The sample size (default: 10000, integer expected);
- The block size (i.e. number of runs launched at the same time), for parallelization purpose
  (default: 1; integer expected). It must be inferior to the maximum sampling size.

In the advanced parameters (default: hidden), the user can set:

- The seed of the random generator (default: 0, positive integer expected).


4-2 Launch
''''''''''

When validating the previous window, a new element appears in the study tree below an item
named **Sensitivity**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analysisContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisWindow.png
    :align: center

Click on the **Start** button launches the analysis. The user can stop the analysis by clicking
on the **Stop** button.

4-3 Results
'''''''''''

When the analysis is finished or stopped, a result window appears.

Sobol' indices
~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol.png
    :align: center

At the left of the result window the section **Outputs** enables the user to choose the result to display.
The results window gathers these tabs:

- The **Indices** tab includes, for a selected output:

  - The first and total order indices and there confidence intervals plotted
    for each input variable.
    The user can configure the plot with the graphic settings (below the 
    study tree);

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobolConfig.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved;

  - A table with the first and total order indices value and there confidence
    intervals for each variable. Each column of indices can be sorted by clicking
    on its header. When sorting the table, the points on the graph are also sorted;

  - The index corresponding to the interactions;

  .. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

  If the Sobol's indices estimates are incoherent, refer to the warning message in the tooltip of |attentionButton|,
  and try to perform the analysis with a greater sample size.
  If the analysis has been launched while the physical model has correlated input variables, a warning
  is added at the bottom of the window.

- The **Summary** tab includes the values of the stop criteria.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol_tab_summary.png
    :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol_tab_parameters1.png
    :align: center

SRC indices
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRC.png
    :align: center

At the left of the result window the section **Outputs** enables the user to choose the result to display.
The results window gathers 2 tabs:

- The **Indices** tab includes, for a selected output:

  - The SRC index plotted for each input variable.
    The user can configure the plot with the graphic settings below the study tree;

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRCConfig.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved;

  - A table with the SRC index value for each variable. Each
    column can be sorted by clicking on its header. When sorting the table, the
    points on the graphic are also sorted.

  If the analysis has been launched while the physical model has correlated input variables, a warning
  is added at the bottom of the window.

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRC_tab_parameters.png
    :align: center

