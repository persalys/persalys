====================================
User manual - Probabilistic analyses
====================================

1- Probabilistic study
======================

In order to perform probabilistic analysis, the user needs first to build a
probabilistic model by using the context menu of the probabilistic study in
the study tree. Only one probabilistic model can be defined by physical model.

The probabilistic model has two tabs:

- the tab **Marginals**
    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelMarginals.png
        :align: center

    The user has to choose which variables are to be modelled by random variables
    thanks to the check box. For random variables, the user has to define which
    distribution they follow.

    On the right, the parameters of the distribution can be filled, together with truncation parameters.
    The values are expected to be floating points or integers, with respect to the distribution support.
    If a wrong value is used, it will return to its previous value and an error message will appear at the bottom
    of the window.

    LogNormal, Weibull and Gumber distributions allows the user to define different type of parameterization.
    To select the parametrization, click on the **Type** select button.

    The user can configure the plot of the distribution and choose between PDF/CDF with the graphic
    settings (below the study tree).

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelMarginalsConfig.png
        :align: center

    The user can define:
      - The Title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis Title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

- the tab **Correlation**

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelCorrelation.png
        :align: center

    The user may define correlation between the variables by modifying the upper triangular
    part of the Spearman correlation matrix. OTGui handles automatically the symmetry of the correlation
    matrix. The Gaussian copula is used to model the dependency. By default all variables are set
    independent. The values are expected to be floating points or integers,
    included in the interval :math:`\left[-1; 1\right]`.


2- Limit state
==============

2-1 Definition
''''''''''''''

A new limit state can be created thanks to the context menu of the previously
created probabilistic model.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitState.png
    :align: center

In the corresponding window, the user has to define the threshold defining the
failure domain corresponding to the selected output. By default it equals 0. The values are expected
to be floating points or integers. The available operators to define the failure event are:

- <= (less or equal);
- >= (greater or equal);
- > (greater than);
- < (less than).

2-2 Threshold exceedance analysis
'''''''''''''''''''''''''''''''''
Definition
~~~~~~~~~~

The user can compute the probability of failure relative to the limit state through
the context menu of the relevant limit state

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityAnalysis.png
    :align: center

The method used is the well-known crude Monte Carlo sampling. The user has to define:

- The sampling size, by default equal to 1000, expected to be an integer;
- The maximum coefficient of variation for the probability, by default equal to 0.1,
  expected to be a floating point or an integer, in the interval :math:`\left[0;1 \right]`.

In the advanced parameters (default: hidden), the user may also set:

- The seed of the random generator (default: 0, positive integer expected);
- The block size (i.e. number of runs launched at the same time), for parallelization purpose
  (default: 1; integer expected).

Results
~~~~~~~

The result window has 3 tabs:

- The tab **Summary** gathers:
    - The output used for the probability estimation;
    - The number of simulations (i.e. calls to the physical model) performed;
    - The failure probability, its coefficient of variation and the corresponding
      confidence interval at 95%.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilitySummary.png
        :align: center
    
- The tab **Histogram** presents the histogram of the output sample.

  The user can configure the histogram plot with the graphic settings
  (below the study tree).

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityHistogram.png
      :align: center

- The tab **Convergence graph** presents the convergence of the Monte Carlo
  algorithm.

  The user can configure the convergence plot with the graphic
  settings (below the study tree).

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityConvergence.png
          :align: center

3- Central tendency analysis
============================

3-1 Definition
''''''''''''''

New central tendency analysis can be created thanks to be context menu of the
probabilistic model. Two methods are available:

- Monte carlo sampling;
- Taylor expansions (second order).

Monte Carlo
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendency.png
    :align: center

The user needs to define the number of simulations. In the advanced parameters,
the user can choose:

- To compute the confidence interval thanks to the checkbox (default: selected)
  at the given level;
- To set the seed of the random generator (default: 0, positive integer expected).

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylor.png
    :align: center


3-2 Results
'''''''''''

Monte Carlo
~~~~~~~~~~~

The results window gathers 7 tabs:

- The tab **Result table** presents the sample generated by the Monte Carlo sampling
  method and the resulting output values.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTable.png
      :align: center

- The tab **Summary** includes, for a selected output:
    - The number of simulations performed;
    - The minimum and maximum output values generated by the Monte Carlo sampling method;
    - The coordinates of the input values which have generated the minimum and maximum
      values of the output;
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

- The tab **PDF/CDF** presents the PDF/CDF (default: PDF) of the output sample
  together with a kernel smoothing representation.

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings (below the study tree).

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPDFConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted output, by default the first output.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPDF.png
      :align: center

- The tab **Box plots** presents the box plot of the output sample.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyBoxplot.png
      :align: center

  The user can configure the box plot with the graphic settings (below the study tree).

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyBoxplotConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted output, by default the first output.

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The tab **Scatter plots** presents the scatter plot of two parameters (Default:
  first output vs first input).

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyScatter.png
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

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyYX.png
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

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyXX.png
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

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylorResults.png
    :align: center

The results window gathers, for a given output:

- The output mean corresponding to the first and second order expansions;
- The standard deviation of the output;
- The variance of the output.

4- Sensitivity analysis
=======================

4-1 Definition
''''''''''''''

New sensitivity analysis can be created thanks to the context menu of the
probabilistic model.

Two types of sensitivity indices are available:

Sobol' indices
~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSobol.png
    :align: center

- The user has to set the sample size (default: 10000, integer expected). The resulting number of
  simulations (i.e. calls to the physical model) is given below;
- In the advanced parameters, the user can choose:
    - The seed of the random generator (default: 0, positive integer expected);
    - The block size (i.e. number of runs launched at the same time), for parallelization purpose
      (default: 1; integer expected).

- the input variables must be independent to perform this analysis.

SRC indices
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSRC.png
    :align: center

- The user has to set the sample size (default: 10000, integer expected);
- In the advanced parameters, the user can choose to set the seed of the random generator (default: 0, positive integer expected).


4-2 Results
'''''''''''

Sobol' indices
~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol.png
    :align: center

The results window gathers:

- The first and total order indices plotted for each input variable.
  The user can configure the plot with the graphic settings (below the 
  study tree);

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobolConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted output, by default the first output;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- A table with the first and total order indices value for each variable. Each
  column can be sorted by clicking on its header. When sorting the table, the
  points on the graphic are also sorted;

- The index corresponding to the interactions;

.. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

If the Sobol's indices estimates are incoherent, refer to the warning message in the tooltip of |attentionButton|,
and try to perform the analysis with a greater sample size.

SRC indices
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRC.png
    :align: center

The results window gathers:

- The SRC index plotted for each input variable.
  The user can configure the plot with the graphic settings (below the 
  study tree);

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRCConfig.png
      :align: center

  The user can define:
    - The Title of the graph, expected to be a string;
    - The plotted output, by default the first output;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis Title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- A table with the SRC index value for each variable.

