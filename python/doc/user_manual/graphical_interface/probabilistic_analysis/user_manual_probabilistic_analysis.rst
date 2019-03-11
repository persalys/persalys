====================================
User manual - Probabilistic analyses
====================================

In order to perform probabilistic analysis, build first a probabilistic model.

.. _probaModel:

1- Probabilistic model
======================

New probabilistic model can be created through:
  - the **Probabilistic model** item in the context menu of the relevant model in the study tree

  .. image:: /user_manual/graphical_interface/physical_model/physicalModelDefinitionContextMenu.png
      :align: center

  - the **Probabilistic model definition** box of the model diagram

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/probaModelBox.png
      :align: center

A physical model contains only one probabilistic model.

At the creation of a probabilistic model, a new item named **Probabilistic model** is added in the
study tree below the physical model item.

Its context menu contains the following actions:
  - **Design of experiments**: Create a new design of experiments
  - **Limit state**: Create a new limit state
  - **Central tendency**: Create a new central tendency analysis
  - **Sensitivity**: Create a new sensitivity analysis

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelMarginals.png
     :align: center

This item is associated with a window which has two tabs:

- the **Marginals** tab:

  - On the left, the user can choose stochastic input variables and their distribution.
      - Variables are deterministic by default: To make a variable stochastic, check the
        variable box.
      - Variable distribution is Gaussian distribution by default. To change the distribution,
        select one among the proposed distributions in the combo box of the **Distribution**
        column.

      .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelDistributionsList.png
          :align: center

      .. |infoButton| image:: /user_manual/graphical_interface/probabilistic_analysis/documentinfo.png

  - The right side allows to:
      - For a deterministic input:
          - show its value (defined in the physical model)
      - For a stochastic input:
          - set up its distribution parameters:
              - click on the icon |infoButton| to see the parameters definition
              - Use floating points or integers, with respect to the distribution support to set up the parameters
                (If a value is not valid: the previous value is used and an error message appears at the bottom
                of the window)
              - Check off **Lower bound** and/or **Upper bound** and set the bounds value
                to truncate the distribution
              - with the **Type** combo box, select the suitable set of parameters to configure the
                distributions: LogNormal, Weibull and Gumbel

              .. image:: /user_manual/graphical_interface/probabilistic_analysis/paramTypeProbaModel.png
                  :align: center

          - show its distribution PDF/CDF
              - The **Graphic settings** window (below the study tree) allows to:
                  - Select the graphic type: PDF or CDF (default: PDF)
                  - Export the figure
                  - Set up parameters:
                      - Graph title (expected: string)
                      - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                        string), minimum/maximum bounds (expected: floating point)

              - Interactions:
                  - Left-click to translate the graph
                  - Mouse wheel up/down to zoom/zoom out

.. _inferenceresultwizard:

  - The proposed distributions list contains the **Inference result** item. If the current study has at least
    an :ref:`inference analysis result <inferenceAnalysis>`, when clicking on
    this item, the following window appears:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/inference_resultWizard.png
        :align: center

    To get a distribution from an inference analysis:
      - look for an analysis result (**Inference analysis** combo box)
      - select a result of a variable (**Variable** combo box)
      - select one distribution in the displayed list. The right side of the window displays
        the current distribution parameters.
        The distributions with which the inference analysis failed (*failed* indicated in the **Acceptation** column) are
        listed for convenience but it is not possible to validate the window when selecting one of them.

    When clicking on the **Finish** button, the chosen distribution is defined for the selected variable.

- the **Dependence** tab allows to set up dependence between variables. It is represented
  by `copulas <http://openturns.github.io/openturns/latest/theory/probabilistic_modeling/copulas.html>`_.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelDependence.png
        :align: center

    On the left, all the stochastic input variables are listed.
    By default no dependence is set between these variables.

    To add dependence:
      - Select at least two variables

        .. image:: /user_manual/graphical_interface/probabilistic_analysis/selectVar_DependenceTab.png
            :align: center

      - Click on the right arrow:
          - these variables are disabled in the first table (a variable can belong to only one group)
          - the set of variables appears in the second table:
              - the default copula is the Normal copula defined by a correlation matrix equal to
                the identity matrix. So, at this step, the variables are still independent.

              .. image:: /user_manual/graphical_interface/probabilistic_analysis/defaultCopula_DependenceTab.png
                  :align: center

        - To change the copula, select one among the proposed copulas in the combo box of the **Copula**
          column. The interface proposed bivariate copulas and the Normal copula.

          .. image:: /user_manual/graphical_interface/probabilistic_analysis/copulasList.png
            :align: center

        - On the right size, set up the copula parameters:
            - click on the icon |infoButton| to see the parameters definition
            - the bivariate copulas are defined with one parameter (expected: floating points or integers)

                .. image:: /user_manual/graphical_interface/probabilistic_analysis/bivariateCopulaParam.png
                    :align: center

            - the Normal copula is defined with a Spearman correlation matrix:
                - modify values on the upper triangular part of the matrix
                  (expected: floating points or integers in the interval :math:`\left[-1; 1\right]`)
                - the symmetry of the correlation matrix is automatically handled
                - the Spearman correlation matrix must be definite positive.
                  If a value is not valid: the previous value is used and an error message appears at the bottom
                  of the window

                .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelCorrelation.png
                    :align: center

            - view the copula PDF/CDF
               - The **Graphic settings** window (below the study tree) allows to:
                  - Select the graphic type: PDF or CDF (default: PDF)
                  - Export the figure
                  - Set up parameters:
                      - Graph title (expected: string)
                      - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                        string), minimum/maximum bounds (expected: floating point)

               - Interactions:
                  - Left-click to translate the graph
                  - Mouse wheel up/down to zoom/zoom out

    To remove dependence:
      - Select a group of variables in the second table
      - Click on the left arrow: these variables are enabled again in the first table


.. _dependenceinferenceresultwizard:

    The copulas list contains the **Inference result** item. If the current study has at least
    a :ref:`dependence inference result <dependenceInference>`, when clicking on
    this item, the following window appears:

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/dependenceInference_resultWizard.png
        :align: center

    To get a copula from an dependence inference analysis:
      - look for an analysis result (**Inference analysis** combo box)
      - select a result of a group of variables (**Group of variables** combo box)
      - select one coula in the displayed list. The right side of the window displays
        the current copula PDF, Kendall plot and parameters.
        The copulas with which the inference analysis failed ('-' indicated in the **Bayesian Information Criterion** column) are
        listed for convenience but it is not possible to validate the window when selecting one of them.

    When clicking on the **Finish** button, the chosen copula is defined for the selected group of variables.

2- Limit state
==============

To perform a reliability analysis, define first a limit state
(possible only if the model contains stochastic input variables).

A new limit state can be created through:
  - the context menu of the probabilistic model item

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelContextMenu.png
        :align: center

  - the **Limit state definition** box of the model diagram

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probaAnalysesBoxes.png
        :align: center

  - the context menu of the **Reliability** item (if it already exists)

When a limit state is required, a new element is added in the study tree below **Reliability**.

Its context menu contains the following actions:
  - **Rename**: Rename the limit state
  - **Threshold exceedance**: Create a new threshold exceedance analysis
  - **Remove**: Remove the limit state and all the analyses depending on it


This item is associated with a window which allows to:
  - select the output to analyse
  - define the failure event by:
      - selecting an operator:
          - <= (less or equal)
          - >= (greater or equal)
          - > (greater than)
          - < (less than)
      - setting up the threshold defining the failure domain (default:  0., expected: floating points or integers)


.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitState.png
    :align: center


3- Threshold exceedance analysis
================================

The user can require a computation of the probability of failure relative to the limit state through:
  - the context menu of the relevant limit state

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateContextMenu.png
        :align: center

  - the **Reliability** box of the model diagram

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/reliabilityBox.png
        :align: center

.. _thresholdexceedancewizard:

3-1 Definition
''''''''''''''

When an analysis is required, the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityAnalysis.png
    :align: center

First, select a method:
  - the simulation methods are:
      - `Monte Carlo <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/monte_carlo_simulation.html>`_
      - FORM - `Importance sampling <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/importance_simulation.html>`_:
        the FORM algorithm starts from the result of the Importance sampling analysis

  - the approximation methods are:
      - First Order Reliability Method (`FORM <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/form_approximation.html>`_)
      - Second Order Reliability Method (`SORM <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/sorm_approximation.html>`_)

Then, according to the method set up the algorithm parameters:

Simulation methods
~~~~~~~~~~~~~~~~~~

The «Simulation methods» window allows to define:
  - the stopping criteria of the algorithm (Select at least one criterion to validate the window):
      - the maximum coefficient of variation for the probability, (default: 0.01,
        expected: a floating point or an integer, in the interval :math:`\left[0;1 \right]`)
      - the maximum computation time (default: one minute)
        (d means days, h means hours, m means minutes, s means seconds)
      - the maximum sampling size (default: 10000, expected: integer)
  - **Evaluation parameter**: the block size – the number of runs launched simultaneously – for parallelization
    purposes (default=1; expected= integer). It must be less than the maximum sampling size.
  - **Advanced Parameters** (default: hidden): the seed of the random generator (default: 0,
    positive integer expected)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilitySimu.png
    :align: center

Approximation methods
~~~~~~~~~~~~~~~~~~~~~

The window allows to define optimization parameters:
    - the algorithm method: Abdo-Rackwitz, Cobyla (default), SQP
    - the starting point (default: means of the distributions of the stochastic inputs)
    - **Advanced Parameters** (default: hidden): the stopping criteria
       - the maximum number of iterations (default: 100, positive integer expected)
       - the errors: absolute, relative, residual and constraint errors (default: 1e-5, positive float expected)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityApprox.png
    :align: center

3-2 Launch
''''''''''

When the window is validated, a new element appears in the study tree below the relevant
limit state item in the **Reliability** section.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/reliabilityWindow.png
    :align: center


3-3 Results
'''''''''''

When the analysis is finished or stopped, a new result window appears which content depends
of the algorithm.

.. _reliamontecarloresult:

Monte Carlo
~~~~~~~~~~~

On the left, the section **Output** reminds the analysed output.

The results window gathers several tabs:

- The **Summary** tab shows:
    - the elapsed computation time
    - the number of simulations (i.e. calls to the model) performed
    - the failure probability estimate, its coefficient of variation and the corresponding
      confidence interval at 95%

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilitySummary.png
        :align: center
    
- The **Histogram** tab presents the histogram of the output sample. The red vertical line
  represents the threshold of the failure event.

  - The **Graphic settings** window (below the study tree) allows to:
      - Export the figure
      - Set up parameters:
          - Graph title (expected: string)
          - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
            string), minimum/maximum bounds (expected: floating point)

  - Graph interactions:
        - Left-click to translate the graph
        - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityHistogram.png
      :align: center

- The **Convergence graph** tab presents the convergence of the probability estimate.

  - The **Graphic settings** window (below the study tree) allows to:
      - Export the figure
      - Set up parameters:
          - Graph title (expected: string)
          - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
            string), minimum/maximum bounds (expected: floating point)

  - Graph interactions:
        - Left-click to translate the graph
        - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityConvergence.png
          :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.


.. _formresult:

FORM
~~~~

On the left, the section **Output** reminds the analysed output.

The results window gathers several tabs:

- The **Summary** tab shows:
    - the failure probability and the Hasofer reliability index
    - the optimization stopping criteria value: 
        - the number of iterations
        - the errors: absolute, relative, residual and constraint errors
    - the number of calls of the model

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMSummary.png
        :align: center

- The **Design point** tab gathers:
    - the coordinates of the design point in the standard space and in the physical space
    - the `importance factors <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/importance_form.html>`_ of each variable

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMDesignPoint.png
        :align: center

- The **Sensitivities** tab gathers:
    - the `sensitivity factors <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/sensitivity_form.html>`_
      of the failure probability and of the reliability index to the parameters
      of the marginals of the stochastic input variables.

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMSensitivities.png
        :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

- The **Model** tab shows the model content and the limit state used to perform the analysis.

.. _sormresult:

SORM
~~~~

The SORM result window contains the same tabs as the FORM result window.

However the **Summary** tab contains additional results:
  - The failure probability and the reliability index computed with the **Breitung** formula;
  - The failure probability and the reliability index computed with the **Hohen Bichler** formula;
  - The failure probability and the reliability index computed with the **Tvedt** formula.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sormResultWindow.png
    :align: center

.. _formisresult:

FORM-IS
~~~~~~~

The FORM-IS result window contains the same tabs as the Monte Carlo result window and a **FORM results**
tab to display the tabs of a FORM result window.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/limitStateReliabilityFORMIS.png
    :align: center

4- Central tendency analysis
============================

New central tendency analysis can be created thanks to:
  - the context menu of the probabilistic model item

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelContextMenu.png
        :align: center

  - the **Central tendency** box of the model diagram

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probaAnalysesBoxes.png
        :align: center

  - the context menu of the **Central tendency** item (if it already exists)

.. _centraltendencywizard:

4-1 Definition
''''''''''''''

When an analysis is required, a window appears, in order to set up:
  - the outputs of interest (**Select outputs** - default: all outputs are analyzed)
  - the method: `Monte Carlo sampling <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/monte_carlo_moments.html>`_ (default) or `Taylor Expansions <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/taylor_moments.html>`_ (second order)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendency.png
    :align: center

Monte Carlo
~~~~~~~~~~~

The «Monte Carlo parameters» window allows to define:
  - the stopping criteria of the algorithm (Select at least one criterion to validate the window):
      - the maximum coefficient of variation for the mean, (default: 0.01,
        expected: a floating point or an integer, in the interval :math:`\left[0;1 \right]`)
      - the maximum computation time (default: one minute)
        (d means days, h means hours, m means minutes, s means seconds)
      - the maximum sampling size (default: 10000, expected: integer)
  - **Evaluation parameter**: the block size – the number of runs launched simultaneously – for parallelization
    purposes (default=1; expected= integer). It must be less than the maximum sampling size.
  - **Advanced Parameters** (default: hidden):
      - the seed of the random generator (default: 0, positive integer expected)
      - require the computation of the confidence interval (default: checked)
        at a given level (default: 0.95, expected: floating point or integer,
        in the interval :math:`\left[0;1 \right[`)


.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyMC.png
    :align: center

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylor.png
    :align: center

4-2 Launch
'''''''''''

When the window is validated, a new element appears in the study tree below **Central tendency**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyWindow.png
    :align: center


4-3 Results
'''''''''''

When the analysis is finished or stopped, a result window appears.

.. _montecarloresult:

Monte Carlo
~~~~~~~~~~~

The **Table** tab, the **Cobweb plot** tab, the **Plot matrix** tab and the **Scatter plots** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

The results window gathers the following tabs:

- The **Summary** tab summarizes the results of the analysis, for a selected variable (left column):
  sample size, minimum/maximum values, input values at
  the extrema of the outputs, moments estimates and empirical quantiles.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencySummary.png
      :align: center

- The **PDF/CDF** tab presents the PDF/CDF of the variables
  together with a `kernel smoothing <http://openturns.github.io/openturns/latest/theory/data_analysis/kernel_smoothing.html>`_ representation.

  - The **Graphic settings** window (below the study tree) allows to:
      - Select the graphic type: PDF or CDF (default: PDF)
      - Export the figure
      - Set up parameters:

        - Graph title (expected: string)
        - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
          string), minimum/maximum bounds (expected: floating point)

  - Interactions:
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPDF.png
      :align: center

- The **Box plots** tab presents the `box plot <https://commons.wikimedia.org/w/index.php?curid=14524285>`_
  of the variables.

  - The **Graphic settings** window (below the study tree) allows to:
      - Export the figure
      - Set up parameters:

        - Graph title (expected: string)
        - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
          string), minimum/maximum bounds (expected: floating point)

  - Interactions:
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyBoxplot.png
      :align: center

- The **Dependence** tab displays the `Spearman's matrix <http://openturns.github.io/openturns/latest/theory/data_analysis/spearman_coefficient.html>`_ estimate.

    - The cells are colored according to the value of the Spearman's coefficient.
    - Its context menu allows to export the table in a CSV file or as a PNG image.
    - Select cells and Press Ctrl+C to copy values in the clipboard

  .. image:: /user_manual/graphical_interface/deterministic_analysis/doe_dependence.png
      :align: center

- The **Table** tab presents the sample generated by the Monte Carlo sampling
  method and the resulting output values. The table can be exported (**Export** button).

  - Interactions:
      - Left-click (optional: + Ctrl) on lines to select them
      - Left-click on column header to sort values in ascending or descending order 
      - Left-click on a column header and drag it in another place to change columns order

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTable.png
      :align: center


- The **Cobweb plot** tab displays all the points generated by the analysis.

  - The **Graphic settings** window (below the study tree) allows to:
      - Select the variables to display and the axis order
      - Modify the space where the values are plotted with the **Ranks** check button
        (default: physical space). Check off the button to display the rank of the values
      - Export the figure

  - Interaction:
      - Left-click on columns to select curves (multiple selection possible)

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyCobweb.png
      :align: center

- The **Plot matrix** tab: histograms of the distribution of each variable (diagonal) and
  scatter plots between each couple of input/output variables (off-diagonal).

  - The **Graphic settings** window (below the study tree) allows to:
      - Select the variables to display and the columns order
      - Export the figure

  - Interactions:
      - Right-click to select points
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyPlotmatrix.png
      :align: center

- The **Scatter plots** tab displays the scatter plot of two parameters.

  - The **Graphic settings** window (below the study tree) allows to:
      - Select the variables to plot on X-axis and Y-axis (default: first output versus first input)
      - Modify the space where the values are plotted with the **Ranks** check button
        (default: physical space). Check off the button to display the rank of the values.
      - Export the figure
      - Set up parameters:

        - Graph title (expected: string)
        - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
          string), minimum/maximum bounds (expected: floating point), log
          scale (check button only available if axis values are positive)
        - Plot style: color, marker size and style (cross, circle, diamond, square, plus)

  - Interactions:
      - Right-click to select points
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyScatter.png
      :align: center


- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendency_MC_tab_Parameters.png
      :align: center

- The **Model** tab shows the model content used to perform the analysis.

.. _taylorresult:

Taylor expansions
~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/centralTendencyTaylorResults.png
    :align: center

The results window gathers, for a selected output (left column):
  - the output mean corresponding to the first and second order expansions
  - the standard deviation of the output
  - the variance of the output

5- Sensitivity analysis
=======================

New sensitivity analysis can be created thanks to:
  - the context menu of the probabilistic model item

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probabilisticModelContextMenu.png
        :align: center

  - the **Sensitivity** box of the model diagram

    .. image:: /user_manual/graphical_interface/probabilistic_analysis/probaAnalysesBoxes.png
        :align: center

  - the context menu of the **Sensitivity** item (if it already exists)

The input variables must be independent to perform a sensitivity analysis.

.. _sensitivitywizard:

5-1 Definition
''''''''''''''

When an analysis is required, a window appears, in order to set up:
  - the outputs of interest (**Select outputs** - default: all outputs are analyzed)
  - the method: `Sobol <http://openturns.github.io/openturns/master/theory/reliability_sensitivity/sensitivity_sobol.html>`_ (default), SRC (= `Standardised Regression Coefficient <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/ranking_src.html>`_)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisMethods.png
    :align: center

Sobol indices
~~~~~~~~~~~~~

The «Sobol parameters» window allows to define:
  - the stopping criteria of the algorithm (Select at least one criterion to validate the window):
      - the maximum confidence interval length of the first order indices (default: 0.01,
        expected: a floating point or an integer, in the interval :math:`\left[0;1 \right]`)
      - the maximum computation time (default: one minute)
        (d means days, h means hours, m means minutes, s means seconds)
      - the maximum calls (default: 10000, expected: integer)
  - **Evaluation parameters**:
      - the replication size (default=1000; expected= integer).
          - The label **Number of calls by iteration** is updated according to its value.
          - At each iteration of the algorithm, the model is evaluated *nbEval* times:
              :math:`nbEval = (nbInputs + 2) * replicationSize`
              with *nbInputs*, the number of stochastic input variables
          - The maximum calls must be greater than *nbEval*
      - the block size – the number of runs launched simultaneously – for parallelization
        purposes (default=1; expected= integer).
  - **Advanced Parameters** (default: hidden):
      - the confidence level (default: 0.95; float expected).
      - the seed of the random generator (default: 0, positive integer expected)


.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSobol.png
    :align: center

See the :ref:`Sensitivity <SobolExample>` section in the example guide.

SRC indices
~~~~~~~~~~~

The «SRC parameters» window allows to define:
  - **Evaluation parameters**:
     - the sample size (default: 10000, integer expected)
     - the block size – the number of runs launched simultaneously – for parallelization
       purposes (default=1; expected= integer). It must be less than the sample size.
  - **Advanced Parameters** (default: hidden): the seed of the random generator
    (default: 0, positive integer expected)

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisDefineSRC.png
    :align: center

5-2 Launch
''''''''''

When the window is validated, a new element appears in the study tree below **Sensitivity**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisWindow.png
    :align: center


5-3 Results
'''''''''''

When the analysis is finished or stopped, a result window appears.

.. _sobolresult:

Sobol indices
~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol.png
    :align: center

The results window gathers these tabs:

- The **Indices** tab includes, for a selected output (left column):

  - the graph of the input variables first and total order indices and there confidence intervals.

    - The **Graphic settings** window (below the study tree) allows to:
       - Export the figure
       - Set up parameters:
          - Graph title (expected: string)
          - X-axis labels orientation
          - Axes title (expected: string)

  - the table of the input variables first and total order indices and there confidence intervals.

      - Interactions:
          - Select cells and Press Ctrl+C to copy values in the clipboard
          - Left-click on column header to sort values in ascending or descending order.
            When sorting the table, the indices are also sorted on the graph.

  - The index corresponding to the interactions.

  .. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

  If the Sobol's indices estimates are incoherent, refer to the warning message in the tooltip of |attentionButton|,
  and try to perform the analysis with a greater sample size.
  If the analysis has been launched while the physical model has correlated input variables, a message
  is added at the bottom of the window to warn the user the result can be false.

- The **Summary** tab includes the values of the stopping criteria.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol_tab_summary.png
    :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSobol_tab_parameters1.png
      :align: center

.. _srcresult:

SRC indices
~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRC.png
    :align: center

The results window gathers these tabs:

- The **Indices** tab includes, for a selected output (left column):

  - the graph of the input variables SRC indices.

     - The **Graphic settings** window (below the study tree) allows to:
         - Export the figure
         - Set up parameters:
             - Graph title (expected: string)
             - X-axis labels orientation
             - Axes title (expected: string)

  - the table of the input variables SRC indices.

      - Interactions:
          - Select cells and Press Ctrl+C to copy values in the clipboard
          - Left-click on column header to sort values in ascending or descending order.
            When sorting the table, the indices are also sorted on the graph.

  If the analysis has been launched while the physical model has correlated input variables, a message
  is added at the bottom of the window to warn the user the result can be false.

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

  .. image:: /user_manual/graphical_interface/probabilistic_analysis/sensitivityAnalysisSRC_tab_parameters.png
      :align: center

