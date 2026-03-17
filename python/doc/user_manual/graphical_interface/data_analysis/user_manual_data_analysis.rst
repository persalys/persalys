.. _data_analysis:

===========================
User manual - Data analysis
===========================

The interface allows one to perform the following types of data analysis:
  - Data analysis to get moments, minimum, maximum, PDF, etc.
  - Sensitivity analysis to get first order Sobol' indices
  - Marginals inferences
  - Dependence inferences
  - Metamodels creation
  - Quantile estimation

1- Data analysis
================

1-1 Creation
''''''''''''

A new sample analysis can be created through:
  - the context menu of the **Definition** item of the data set

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Data analysis** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

When the analysis is required, a new item is added in the study tree below the data set item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Remove**: Remove the analysis from the study

This item is associated with a window showing a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisWindow.png
    :align: center

.. _dataanalysisresult:

1-2 Results
'''''''''''

When the analysis is finished or stopped, the following window appears.

.. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_summary.png
    :align: center

The window shows numerous tabs, some of which are interactively linked (**Table**,
**Parallel coordinates plot**, **Plot matrix** and **Scatter plot** tabs):
when the user selects points on one of these representations, the same points are
automatically selected in the other tabs.

- The **Summary** tab summarizes the results of the analysis, for a selected variable (left column):
  sample size, elapsed time, moment estimates, empirical quantiles, minimum/maximum values, input
  values at extremum.

- The **PDF/CDF** tab presents the PDF/CDF of the variables
  together with a `kernel smoothing <http://openturns.github.io/openturns/latest/theory/data_analysis/kernel_smoothing.html>`_ representation.

  - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters and
    select the graphic type: PDF (default) or CDF
  - Graph interactivity:
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom in/zoom out

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_PDF.png
      :align: center

- The **Box plots** tab presents the `box plot <https://commons.wikimedia.org/w/index.php?curid=14524285>`_
  of the variables.
  They are rescaled for each variable (:math:`x`), using mean (:math:`\mu`) and standard deviation (:math:`\sigma`): :math:`y = (x - \mu)/\sigma`

  - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
  - Graph interactivity:
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom in/zoom out

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_boxplot.png
      :align: center

.. _dependenceTab:


- The **Dependence** tab displays the `Spearman's matrix <http://openturns.github.io/openturns/latest/theory/data_analysis/spearman_coefficient.html>`_ estimate.

    - The cells are colored according to the value of the Spearman's coefficient.
    - Its context menu allows one to export the table in a CSV file or as a PNG image.
    - Select cells and Press Ctrl+C to copy values in the clipboard

  .. image:: /user_manual/graphical_interface/deterministic_analysis/doe_dependence.png
      :align: center

- The **Table** tab shows the input/output samples. The table can be exported (**Export** button).

  - Table interactivity:
      - Left-click on lines to select them (optional: + Ctrl for adding to current selection, + Shift for contiguous selection)
      - Left-click on column header to sort values in ascending or descending order
      - Left-click on a column header and drag it in another place to change columns order
      - Right-click on table to export or copy to the clipboard (shortcut Ctrl+C) the current selection
      - Ctrl+A selects all lines

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTable.png
      :align: center


- The **Parallel coordinates plot** tab displays the sample points.

  - Use the :ref:`Graph settings <firstgraphsettings>` window to set up graphical parameters.
  - Graph interactivity:
      - Left-click on columns to select curves (multiple selection possible)

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_Cobweb.png
      :align: center


- The **Plot matrix** tab: histograms of the distribution of each variable (diagonal) and
  scatter plots between each couple of input/output variables (off-diagonal).

  - Use the :ref:`Graph settings <firstgraphsettings>` window to set up graphical parameters.
  - Graph interactivity:
      - Right-click to select points
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom in/zoom out

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX.png
      :align: center

- The **Scatter plots** tab displays the scatter plot of two parameters.

  - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters and
    select the variables to plot on X-axis and Y-axis (default: first output versus first input)
  - Graph interactivity:
      - Right-click to select points
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom in/zoom out

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_scatterplot.png
      :align: center

.. _sensitivityAnalysis:

2- Sensitivity analysis
=======================

The sensitivity analysis allows one to assess the influence of input variables on output variables.

Several methods are available, split into two categories:

**Global sensitivity analysis (GSA) methods:**

- **Rank Sobol' indices**: Non-parametric first order Sobol' indices computed using the
  :py:class:`openturns.experimental.RankSobolSensitivityAlgorithm`. These indices measure
  any monotonic relationship between inputs and outputs since they are calculated on ranks.

- **Standard regression coefficients (SRC)**: Signed and squared SRC indices that measure
  linear relationships between an output and the input vector. A coefficient of determination
  :math:`R^2` is also provided to assess the quality of the linear approximation.

- **Global HSIC indices**: The Hilbert-Schmidt Independence Criterion (HSIC) measures the
  dependence between an input variable and an output variable through kernel-based statistics.
  See `HSIC sensitivity indices <http://openturns.github.io/openturns/latest/theory/reliability_sensitivity/sensitivity_hsic.html>`_.

**Reliability-oriented sensitivity analysis (ROSA) methods:**

- **Target HSIC indices**: A variant of HSIC that focuses on a specific region of interest in
  the output space defined by a filter function. This is useful when one is interested in the
  sensitivity of the model with respect to extreme or critical output values.

- **Conditional HSIC indices**: Another ROSA variant that uses weight functions to focus
  the sensitivity analysis on specific output regions. Unlike Target HSIC, this method
  does not support asymptotic p-values or the U-statistic estimator.

2-1 Creation
''''''''''''''
A new sensitivity analysis can be created through:
  - the context menu of the **Definition** item of the data set

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Data analysis** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

When the analysis is required, a new item is added in the study tree below the data set item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Remove**: Remove the analysis from the study

This item is associated with a window showing a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/dataSensitivityAnalysisWindow.png
    :align: center

.. _datasensitivityanalysiswizard:

2-1-1 Wizard
~~~~~~~~~~~~~

The wizard allows one to configure the sensitivity analysis:

- **First page** — Method selection:

  - Select the output variable(s) of interest.
  - Select one or more analysis methods from the two groups (GSA and ROSA).
    By default, Rank Sobol' and SRC are selected.
  - If any HSIC method is checked (Global, Target, or Conditional), the wizard
    will show additional pages to configure their parameters.

- **HSIC parameters pages** (one per selected HSIC type):

  Each HSIC parameters page allows one to configure:

  - **Statistic type** (Global and Target HSIC only): Choose between V-statistic
    (biased but faster, asymptotically unbiased) and U-statistic (unbiased but
    more computationally expensive).
  - **P-values computation**:

    - Permutation p-values (available for all HSIC types).
    - Asymptotic p-values (available for Global and Target HSIC only).

  - **Covariance models**: A table listing the covariance model for each input and
    output variable. The default is the Squared Exponential model.
  - **Filter functions** (Target HSIC only): An alpha parameter table for each output
    variable of interest. The filter function is constructed as
    :math:`x \mapsto \exp\left(-x / (\alpha_i \cdot \sigma_i)\right)` where :math:`\sigma_i`
    is the standard deviation of the :math:`i`-th output variable.
  - **Weight functions** (Conditional HSIC only): An alpha parameter table for each
    output variable of interest, with the same functional form as the filter functions.

.. _datasensitivityanalysisresult:

2-2 Results
'''''''''''''

When the analysis is finished or stopped, the following window appears.

.. image:: /user_manual/graphical_interface/data_analysis/dataSensitivityAnalysisResultWindow.png
    :align: center

On the left, you can select the output variable.
The result window shows one tab per selected analysis method:

- The **Rank Sobol'** tab shows the first order Sobol' indices for each input variable
  in both the graph and the table. You can sort the table by any column by clicking
  on the column header. The graph will be sorted in the same way as the table.

- The **SRC** tab displays signed and squared SRC indices in the same way and the
  :math:`R^2` coefficient at the top. If :math:`R^2 < 0.8`, a warning is displayed
  indicating that the model might not be linear enough for SRC indices.

- The **Global HSIC**, **Target HSIC** and/or **Conditional HSIC** tabs (depending on
  the selected methods) each contain sub-tabs for:

  - HSIC indices
  - R2-HSIC indices
  - Permutation p-values (if computed)
  - Asymptotic p-values (if computed)

  Each sub-tab shows a bar chart and a table of the indices for all input variables.

If the Spearman test detects a dependency between two variables, a warning will be displayed at the top of the window.
Remember that both Sobol' and SRC indices are only valid for independent variables.
Always ensure that the input variables are independent before interpreting the sensitivity indices.
SRC indices only measure linear relationship between an output and the input vector.
Since they are in that case calculated on ranks, Sobol' indices can measure any monotonic relationship.
HSIC indices are valid for independent variables and can detect any type
of dependence, including non-monotonic relationships. For dependant variables, they mesure a total dependence that includes both direct and indirect effects.

.. _inferenceAnalysis:

3- Marginals inference
======================

The inference analysis allows one to perform a Bayesian Information Criterion
(`BIC <http://openturns.github.io/openturns/latest/theory/data_analysis/bic.html>`_) and
either a `Kolmogorov-Smirnov <http://openturns.github.io/openturns/latest/theory/data_analysis/kolmogorov_test.html>`_ or `Lilliefors <http://openturns.github.io/openturns/latest/auto_data_analysis/statistical_tests/plot_kolmogorov_test.html#case-2-the-distribution-parameters-are-estimated-from-the-sample>`_ goodness-of-fit tests for 1-d continuous distributions.

New marginals inference can be created thanks to:
  - the context menu of the **Definition** item of the data set

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Marginals inference** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _marginalsinferencewizard:

3-1 Definition
''''''''''''''

.. image:: /user_manual/graphical_interface/data_analysis/inference_wizard.png
    :align: center

When an analysis is required, a window appears, in order to set up:
  - the variables of interest (default: all variables are analysed) by checking off
    the corresponding line in the first table
  - the list of distributions to infer for each variable (default: Normal distribution):
      - The list of distributions can be different for each variable.
      - Click on **Apply the list of distributions to all variables**
        in the context menu of a variable to set up the same list of distributions
        to the other checked variables.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_wizard_applyToAll.png
            :align: center

      - To add a distribution, click on the **Add** combo box and select a distribution of the list
        which appears (or all of them with the **All** item):

        - the distribution is added in the table
        - the distribution is removed from the combo box

        .. image:: /user_manual/graphical_interface/data_analysis/inference_wizard_distributions_list.png
            :align: center

      - To remove a distribution, select it in the table and click on **Remove**.
        Press the Ctrl or Shift key to select multiple lines.
  - the Kolmogorov-Smirnov/Lilliefors level such that :math:`\alpha = 1 - {\rm level}` is the risk of
    committing a Type I error, that is an incorrect rejection of a true null hypothesis
    (default: 0.05., expected: float in the range :math:`]0, 1[`)
  - **Advanced paramters** are as follows:

    - Require an estimation of the tested distributions parameters confidence interval at a specified level
    - Fine-tune Lilliefors parameters (precision, min/max sampling sizes)



3-2 Launch
''''''''''

When the analysis is required, a new item is added in the study tree below the data set item.

Its context menu has two actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/inferenceWindow.png
    :align: center

.. _marginalsinferenceresult:

3-3 Results
'''''''''''

When the analysis is finished or stopped, a window appears.

.. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_PDF.png
    :align: center

The results window gathers:

- The **Summary** tab includes, for a selected variable (left column):
   - a table of all the tested distributions, the associated Bayesian Information Criterion value and the p-value.
       - The last column indicates whether the distribution is accepted or not according to the given level.
       - The distributions are sorted in increasing order of BIC values.
   - for the selected distribution:
      - The **PDF/CDF** tab presents the PDF/CDF of the sample
        together with the distribution PDF.

        - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters and
          select the graphic type: PDF (default) or CDF
        - Graph interactivity:
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom in/zoom out

      - The **Q-Q plot** tab presents the `Q-Q plot <http://openturns.github.io/openturns/latest/theory/data_analysis/graphical_fitting_test.html>`_
        which opposes the data quantiles to the quantiles of the tested distribution.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_QQplot.png
           :align: center

        - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
        - Graph interactivity:
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom in/zoom out

      - The **Parameters** tab includes a table with the moments of the selected distribution
        and the values
        `estimate <http://openturns.github.io/openturns/latest/theory/data_analysis/parametric_estimation.html>`_
        of its native parameters.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters.png
           :align: center

        *failed* in the **Acceptation** column means that an error occurred when building a distribution
        with the given sample. Then, the **Parameters** tab shows the error message.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters_error_message.png
            :align: center


The result can be used in the :ref:`Probabilistic model window <inferenceresultwizard>`.

.. _dependenceInference:

4- Dependence inference
=======================

The dependence inference allows one to infer copulas on the sample of the data set.

This analysis can be created thanks to:
  - the context menu of the **Definition** item of the relevant data set

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Dependence inference** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _dependenceinferencewizard:

4-1 Definition
''''''''''''''

When an analysis is required, a window appears:

.. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizard.png
    :align: center

The windows allows one to set up:
  - the groups of variables to test:
      - Select at least two variables of the model (left table):
          - Refer to the estimate of the :ref:`Spearman's matrix <dependenceTab>` in
            the data analysis result window to create groups
          - For convenience, the list of groups may be set by default thanks to this estimate
            (if correlation between variables exists)
      - Click on the right arrow:
          - the group is added in the second table
          - a third table appears with the default item *Normal*

  .. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizardOneGroup.png
    :align: center

  - the copulas to infer on the groups:
    - Click on the **Add** combo box
    - Select a copula in the list (or all of them with the **All** item):

        - *For a pair of variables* : bivariate copulas are available
          (Ali-Mikhail-Haq, Clayton, Farlie-Gumbel-Morgenstern, Frank, Gumbel, Normal)
        - *For a group with more than two variables*: only the Normal copula is available
          (**Add** and **Remove** buttons are then disabled)

    .. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizard_copulaList.png
      :align: center

To remove a group:
  - Select a group in the second table
  - Click on the left arrow

4-2 Launch
'''''''''''

When the analysis is required, a new item is added in the study tree below the data set item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/copulaInferenceWindow.png
    :align: center

.. _dependenceinferenceresult:

4-3 Results
'''''''''''

When the analysis is finished or stopped, a window appears:

.. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_PDF.png
    :align: center

The window gathers:

- The **Summary** tab includes, for a selected set of variables:
    - a table of all the tested copulas
    - for the selected copula:
        - the **PDF/CDF** tab presents, for each pair of variables, the PDF/CDF of the sample
          together with the distribution PDF.

            - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters and
              select the graphic type: PDF (default) or CDF
            - Graph interactivity:
                - Left-click to translate the graph
                - Mouse wheel up/down to zoom in/zoom out

        - the **Kendall plot** tab presents a visual fitting test for each pair of variables using the
          `Kendall plot <http://openturns.github.io/openturns/latest/theory/data_analysis/graphical_fitting_test.html>`_.
          This plot can be interpreted as a QQ-plot (for marginals): the more the curve fits
          the diagonal, the more adequate the dependence model is.

            - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
            - Graph interactivity:
                - Left-click to translate the graph
                - Mouse wheel up/down to zoom in/zoom out

        .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_Kendall.png
            :align: center

        - the **Parameters** tab includes the parameters `estimate <http://openturns.github.io/openturns/latest/theory/data_analysis/parametric_estimation.html>`_ of the selected copula.

            .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters.png
                :align: center

            - *For the Gaussian copula*: the tab displays the `Spearman's coefficients <http://openturns.github.io/openturns/latest/theory/data_analysis/spearman_coefficient.html>`_.


            - *'-'* in the **BIC** column means that an error occurred when building a copula
              with the given sample. Then, the **Parameters** tab shows the error message.

            .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters_ErrorMessage.png
                :align: center

The result can be used in the :ref:`Probabilistic model window <dependenceinferenceresultwizard>`.

5- Metamodel creation
======================

To perform this analysis, the data set or the design of experiments must contain an output sample.

A new metamodel can be created in 4 different ways:
  - the context menu of a design of experiments item

    .. image:: /user_manual/graphical_interface/data_analysis/doe_eval_ContextMenu.png
        :align: center

  - the **Metamodel creation** box of a physical model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/physicalModel_Diagram_metamodelBox.png
        :align: center

  - the context menu of the **Definition** item of a data set

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Metamodel creation** box of a data set diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _metamodelwizard:

5-1 Definition
''''''''''''''

When an analysis is required, a window appears, in order to set up:
  - the outputs of interest (**Select outputs** - default: all outputs are analyzed)
  - the method: polynomial regression (default), functional chaos or Gaussian process

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_wizard.png
    :align: center

5-1-1 Linear regression
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **Linear regression** window allows one to define:
  - **Parameters**: polynomial degree (default: 1, expected: integer in [1, 2]), interaction terms (if degree>1 only)

Refer to :class:`~persalys.PolynomialRegressionAnalysis` for implementation details.

5-1-2 Functional chaos
~~~~~~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_functional_chaos_wizard.png
    :align: center

The **Functional chaos parameters** window allows one to define:
  - **Parameters**: chaos degree (default: 2, expected: integer greater or equal to 1)
  - **Advanced Parameters** (default: hidden): sparse chaos (default: not sparse)

Refer to :class:`~persalys.FunctionalChaosAnalysis` for implementation details.

5-1-3 Gaussian Process
~~~~~~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_kriging_wizard.png
    :align: center

The **Gaussian Process parameters** window allows one to define:
  - **Parameters**:
     - *The type of covariance model*: Squared exponential (default), Absolute exponential,
       Generalized exponential, Matern model
     - *Parameters of the covariance model* (default: hidden, visible if a model is chosen):
         - **Generalized exponential**: parameter **p**,
           exponent of the euclidean norm (default: 1., positive float expected)

         .. image:: /user_manual/graphical_interface/data_analysis/kriging_p_parameter.png
              :align: center

         - **Matern**: coefficient **nu** (default: 1.5, positive float expected)

         .. image:: /user_manual/graphical_interface/data_analysis/kriging_nu_parameter.png
              :align: center

     - *The type of the trend basis*: Constant (default), Linear or Quadratic
  - **Advanced Parameters** are accessible for model covariance optimization (default: hidden):
     - Optimize the covariance model parameters (default: checked)
     - Scales for each input (default: 1): To edit the scales, click on the "**...**" button to
       generate the input variables table and their scale through a wizard.

     .. image:: /user_manual/graphical_interface/data_analysis/kriging_scale_wizard.png
          :align: center

     - Amplitude of the process (default: 1., positive float expected)

Refer to :class:`~persalys.KrigingAnalysis` for implementation details.

5-1-3 Validation
~~~~~~~~~~~~~~~~

In the following window, the generated metamodel can be validated, with three different methods:
  - *Analytically* (default): This method corresponds to an analytical evaluation of the Leave-one-out method result.
    Note that this method is not available when the metamodel involves selection, like functional chaos with the *sparse* option enabled.
  - *Using a test sample*: The data sample is divided into two subsamples, by picking points randomly (default seed = 1):
    training sample (default: 80% of the sample points) and test sample
    (default: 20% of the sample points).
    A new metamodel is built with the training sample and is validated with the test sample.
  - *Using the K-Fold method*:
    Define the number of folds (default: 5, expected: integer greater than 1) and specify how the
    folds are generated (default seed:1).

See more details on cross-validation `here <https://openturns.github.io/openturns/latest/theory/meta_modeling/cross_validation.html>`_.

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_validation_page.png
    :align: center

5-2 Results
'''''''''''

When the window is validated, a new element appears in the study tree below the
data set item or the design of experiments item.

The context menu of this item contains these actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Convert metamodel into physical model** (default: disabled,
    enabled when the analysis is successfully finished): Add the
    metamodel in the study tree
  - **Export metamodel** (default: disabled, enabled when the analysis
    is successfully finished): Export the metamodel in a standalone
    OpenTURNS study file (XML) which can be accessed by running the
    generated python script
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/metaModelWindow.png
    :align: center

.. _functionalchaosresult:

5-2-1 Functional chaos
~~~~~~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_moments.png
    :align: center

The results window gathers:

- The **Results** tab shows different information about the selected output (left column):
    - `MSE <https://openturns.github.io/openturns/latest/user_manual/response_surface/_generated/openturns.MetaModelValidation.html#openturns.MetaModelValidation.computeMeanSquaredError>`_
    - `R2  <https://openturns.github.io/openturns/latest/user_manual/response_surface/_generated/openturns.MetaModelValidation.html#openturns.MetaModelValidation.computeR2Score>`_
    - first and second order moments
    - polynomial basis: dimension, maximum degree, full/truncated size
    - part of variance explained by each polynom


.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_plot.png
    :align: center

- The **Adequation** tab shows the fitting curve between the physical
  model output values (**Real otput values**) and the metamodel values
  (**Prediction**). The reference diagonal (in black) is built with the
  physical model output values.

  - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
  - Graph interactivity:

    - Left-click to translate the graph
    - Mouse wheel up/down to zoom in/zoom out

- The **Sobol indices** tab includes, for a selected output (left column):

  - The graphic representation of the first and total order indices for each variable.
    Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
  - A summary table with the first and total order indices.

      - Table interactivity:
          - Select cells and Press Ctrl+C to copy values in the clipboard
          - Left-click on column header to sort values in ascending or descending order.
            Sorting the table will automatically sort the indices on the graph.

  - The index corresponding to the interactions (below the table).

  .. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

  If the Sobol's indices estimates are incoherent, an |attentionButton| will appear in the table.
  It is advised to refer to the associated warning message (tooltip of the |attentionButton|).

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_sobol_indices.png
    :align: center

- The **Validation** tab (default: hidden; visible if a metamodel validation is required) shows for each method and selected output:
    - The metamodel predictivity coefficient: :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)^2}}`
    - The residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
    - *K-Fold* and *Test sample*: A plot showing the relation between the output values (physical
      model) and the predicted metamodel values. The relation is compared to a reference
      diagonal built with the physical model output values.

        - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
        - Graph interactivity:
           - Left-click to translate the graph
           - Mouse wheel up/down to zoom in/zoom out

      .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
         :align: center

    - *Analytical*: the Q2 value

      .. image:: /user_manual/graphical_interface/data_analysis/FC_analyticalValidation.png
         :align: center


- The **Parameters** tab summarizes the parameters of the metamodel creation.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_parameters.png
      :align: center

.. _krigingresult:

5-2-2 Gaussian Process
~~~~~~~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_plot.png
    :align: center

The results window gathers:

- The **Metamodel** tab shows for a selected output the graphic relation between output values from
  the physical model (**Real output values**) and metamodel values (**Prediction**).
  The reference diagonal (in black) is built with the physical model output values.

  - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
  - Graph interactivity:
         - Left-click to translate the graph
         - Mouse wheel up/down to zoom in/zoom out

- The **Results** tab presents the optimized covariance model parameters and the trend coefficients.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_results.png
      :align: center

- If a metamodel validation is required, a **Validation** tab appears for the selected method and output:
    - The residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
    - The metamodel predictivity coefficient: :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)^2}}`
    - A plot showing the relation between the output values (physical
      model) and the predicted metamodel values. The relation is compared to a reference
      diagonal built with the physical model output values.

      - Use the :ref:`Graph settings <secondgraphsettings>` window to set up graphical parameters.
      - Graph interactivity:
           - Left-click to translate the graph
           - Mouse wheel up/down to zoom in/zoom out

      .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
         :align: center

- The **Parameters** tab summarizes the parameters of the metamodel creation.

5-3 Export
''''''''''

5-3-1 Export as a physical model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can export the metamodel as a physical model to perform analyses on it as with any physical model.
To do so, right-click on the metamodel analysis and select **Convert metamodel into physical model**.
If the metamodel is based on a physical model, the probabilistic model of the inputs is exported with the metamodel.
If the metamodel is a functional chaos metamodel built from a data set, the probabilistic model inferred during the analysis is exported as well.

.. image:: /user_manual/graphical_interface/data_analysis/convert_into_physical_model.png
  :align: center

5-3-2 Export as a python model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The metamodel can also be embedded inside a Python model to add a pre- or post-processing step.
To do this, right-click on the metamodel analysis and select **Convert metamodel into python model**.

.. image:: /user_manual/graphical_interface/data_analysis/convert_into_python_model.png
  :align: center


6- Quantile estimation
======================

Quantile estimation can be performed on any marginal of a DataModel
via two methods:

- Monte Carlo : Marginal distribution is inferred using a Kernel Smoothing (KS) distribution
- Generalized Pareto : Marginal tail distribution is inferred using extreme value theory

Inferred distribution are then used to estimate user-specified
quantiles and their associated confidence intervals.

.. _quantileanalysis:

6-1 Definition
''''''''''''''

Quantile estimation analysis can be created thanks to:
  - the context menu of the **Definition** item of the data set

    .. image:: /user_manual/graphical_interface/data_analysis/quantileAnalysisdefContextMenu.png
        :align: center

  - the **Quantile analysis** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

From there, the user can choose:
- on which marginal(s) quantiles will be estimated marginal

- which method will be used

  - **Monte Carlo** (MC)

  - **Excess with Generalized Pareto distribution** (GPD)

    .. image:: /user_manual/graphical_interface/data_analysis/quantileFirstPage.png
        :align: center

- the next steps consists in defining the distribution tails and
  probabilities associated the quantiles to estimate (:math:`q`).

  - target probabilities (:math:`P_t`) can be defined:

    - globally defined using the topmost field

    - individually by typing in the cell or by clicking the [...] button on each row

    - for the **Monte Carlo** method, the sample size **must** be
      large enough to ensure quantile validity. Minimum required
      sample size is dependent on the target probability and is given
      by a method based on
      `QuantileConfidence <https://openturns.github.io/openturns/latest/user_manual/_generated/openturns.QuantileConfidence.html>`_

  - tails can be chosen by clicking the corresponding checkboxes

    - lower tail: will estimate :math:`q_{low}`, such as, :math:`P(X<q_{low}) = P_t`

    - upper tail: will estimate :math:`q_{up}`, such as, :math:`P(X>q_{up}) = P_t`

    - bilateral : will estimate both :math:`q_{low}` and :math:`q_{up}`,
      such as :math:`P(X<q_{low} \cup X>q_{up}) = P_t`, assuming :math:`P(X<q_{low}) = P(X>q_{up}) = P_t/2`

    .. image:: /user_manual/graphical_interface/data_analysis/quantileSecondPage.png
        :align: center

- if **Excess with Generalized Pareto distribution** has been chosen,
  the analysis definition shows a third and final page to specify tail distribution(s)

  - This page consists in two tabs:

    - **Threshold**: the user can specifiy the quantile/probabillity
      below/beyond which the marginal excess sample is defined. The
      excess sample size is also displayed.

    - **Mean excess**: displays a plot for each marginal and each side
      of the distribution, aimed at helping the user to choose an
      appropriate threshold. See this `example <https://openturns.github.io/openturns/latest/auto_data_analysis/distribution_fitting/plot_estimate_gpd_dowjones.html>`_ for more details.

    .. image:: /user_manual/graphical_interface/data_analysis/quantileThirdPage1.png
        :align: center

    .. image:: /user_manual/graphical_interface/data_analysis/quantileThirdPage2.png
        :align: center


.. _quantileresult:

6-2 Results
'''''''''''

When the analysis is finished, the following window appears, presenting two tabs:

- **Quantiles** tab displays a table for each of the selected
  marginals, containing the estimated quantiles.

  - If **Excess with Generalized Pareto distribution** has been
    chosen, there is an additional table displaying the
    Kolmogorov-Smirnov fitting test P-value between the Generalized
    Pareto distribution and the excess sample.

    .. image:: /user_manual/graphical_interface/data_analysis/quantile_analysis_summary.png
        :align: center

- **CDF** tab displays for each tail (lower/upper):

  - the empirical CDF/Survival function of the marginal sample (in black)

  - the estimated quantile(s) and its/their confidence interval(s) (in blue)

  - the excess-inferred Generalized Pareto (if any, in red)

    .. image:: /user_manual/graphical_interface/data_analysis/quantile_analysis_summary2.png
        :align: center
