===========================
User manual - Data analyses
===========================

The OTGui allows the user to launch the following types of data analyses:

- Data analyses to get moments, minimum, maximum, PDF, etc;
- Marginals inferences;
- Dependence inferences;
- Metamodels creation

1- Data analysis
================

1-1 Creation
''''''''''''

New sample analysis can be created through:

- the context menu of the **Definition** item of the relevant data model;
- the **Data analysis** box of the model diagram

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Remove**: Remove the analysis from the study.

This item is associated with a window displaying a progress bar and the buttons
**Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisWindow.png
    :align: center

Click on the **Start** button enables to launch the analysis. The user can stop the evaluation by
clicking on the **Stop** button.

1-2 Results
'''''''''''

When the analysis is finished or stopped, the below window appears.

.. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_summary.png
    :align: center

At the left of the result window the section **Variables** enables the user to choose the result to display.
The right side of the window contains 4 tabs presenting the analysis of the outputs.
The **Table** tab, the **Cobweb plot** tab, the **Plot matrix** tab and the **Scatter plots** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

- The **Summary** tab includes, for a selected variable:
    - The number of values;
    - The minimum and maximum values;
    - If the variable is an output, the coordinates of the input values which have generated
      the minimum and maximum values;
    - Statistics of the variable distribution:
        - Mean;
        - Standard deviation;
        - Coefficient of variation;
        - Skewness;
        - Kurtosis;
        - First quartile;
        - Third quartile;
    - The probability (resp. quantile) can be defined to compute the corresponding
      quantile (resp. probability) of the sample distribution.

- The **PDF/CDF** tab presents the PDF/CDF (default: PDF) of the sample
  together with a kernel smoothing representation.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_PDF.png
      :align: center

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings below the study tree.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_PDF_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Box plots** tab presents the box plot of the sample.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_boxplot.png
      :align: center

  The user can configure the box plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_boxplot_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

.. _dependenceTab:

- The **Dependence** tab displays the estimate of the Spearman's matrix.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_dependence.png
      :align: center

- The **Table** tab:

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_Table.png
      :align: center

- The **Cobweb plot** tab:

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_Cobweb.png
      :align: center

  The user can select the variables to show and the order of the axes with the graphic settings
  below the study tree. The **Export** button opens a dialog box for the user to select where the
  figure has to be saved.

- The **Plot matrix** tab displays the plot matrix which gathers:
  - Out of the diagonal: the scatter plot of each couple of variables (inputs and outputs);
  - On the diagonal: the histogram of the distribution of each variable.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX.png
      :align: center

- The **Scatter plots** tab presents the scatter plot of two parameters (Default:
  first output vs first input).

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_scatterplot.png
      :align: center

  The user can configure the scatter plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/deterministic_analysis/scatterGraphSetting.png
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

.. _inferenceAnalysis:

2- Marginals inference
======================

The inference analysis allows to perform a Bayesian Information Criterion (BIC) and
a Kolmogorov goodness-of-fit tests for 1-d continuous distributions.

New marginals inference can be created thanks to:
  - the context menu of the data model **Definition** item
  - the **Marginals inference** box of the data model diagram

2-1 Definition
''''''''''''''

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/data_analysis/inference_wizard.png
    :align: center

The user needs to choose the variables of interest of the design of experiments by checking
the corresponding lines in the table at the left side of the window. By default, all of them
are selected. To validate the window, at least one variable must be selected.

The table at the right side of the window is used to list the distributions
to test for each variable. By default, only the Normal distribution is tested.
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
null hypothesis. Its value is a float in the range :math:`]0, 1[` and is by default 0.05.


2-2 Launch
''''''''''

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has two actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analysisContextMenu.png
    :align: center

This item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/data_analysis/inferenceWindow.png
    :align: center

Click on the **Start** button enables to launch the analysis. The user can stop the evaluation by
clicking on the **Stop** button.

2-3 Results
'''''''''''

When the analysis is finished or stopped, the below window appears.

.. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_PDF.png
    :align: center

The results window gathers two tabs for each variable:

- The **Summary** tab includes, for a selected variable, a table of all the tested
  distributions, the associated Bayesian Information Criterion value and the p-value. The last column
  indicates whether the distribution is accepted or not according to the given level.
  The distributions are sorted in increasing order of BIC values.
  The right side of this table is updated according to the distribution which is selected.

  This section contains 3 tabs:

  - The **PDF/CDF** tab presents the PDF/CDF (default: PDF) of the sample
    together with the distribution PDF.

  The user can configure the plot and choose between PDF/CDF with the graphic
  settings (below the study tree).

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_PDF_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  - The **Q-Q plot** tab presents the Q-Q plot which opposes the data quantiles to the quantiles
    of the tested distribution.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_QQplot.png
      :align: center

  The user can configure the Q-Q plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_qqplot_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  - The tab **Parameters** includes a table with the moments of the selected distribution
    and the values of its native parameters.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters.png
      :align: center

  - If an error occured when building a distribution with the given sample, 'failed' is indicated in the
    acceptance column, the first two tabs are disabled and the **Parameters** tab displays the error message.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters_error_message.png
      :align: center

- The tab **Parameters** indicates the level used to perform the inference.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_parameters.png
      :align: center


.. _dependenceInference:

3- Dependence inference
=======================

The dependence inference allows to infer copulas on the input sample
of the data model.

This analysis can be created thanks to:
  - the context menu of a data model **Definition** item
  - the **Dependence inference** box of the model diagram.

3-1 Definition
''''''''''''''

When requiring this analysis the following window appears to define its parameters.

.. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizard.png
    :align: center

This window enables to define group of variables and the copulas to test for each of them.

The left side of the window lists all the variables of the model in a table.
The right side list the groups of variables.

To add a group, select at least two variables in the first table and click on the right arrow.
To remove a group, select a group in the second table and click on the left arrow.

The user can refer to the estimate of the :ref:`Spearman's matrix <dependenceTab>`
in the result window of the data analysis to create the groups.
For convenience the list of groups may be set by default thanks to this estimate
if correlation between variables exists.

.. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizardOneGroup.png
    :align: center

When the group is adding, a new table appears with the **Normal** item.
Below this table there are two buttons **Add** and **Remove** to add and remove items from
the list.

If the group has two variables the `available copulas <http://openturns.github.io/openturns/master/user_manual/_generated/openturns.Copula.html>`_ are:
  - Ali-Mikhail-Haq
  - Clayton
  - Farlie-Gumbel-Morgenstern
  - Frank
  - Gumbel
  - Normal

If the group has more than two variables only the Normal copula is available. That is why the
buttons **Add** and **Remove** are disabled in this case.

3-2 Launch
'''''''''''

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

This item is associated with a window displaying a progress bar and the buttons
**Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisWindow.png
    :align: center

Click on the **Start** button enables to launch the analysis. The user can stop the evaluation by
clicking on the **Stop** button.

3-3 Results
'''''''''''

The results window gathers the **Summary** tab for each set of variables.

- The **Summary** tab includes, for a selected set of variables, a table of all the tested
  copulas. The right side of this tab is updated according to the copula which is selected.

  This section contains the following tabs:

  - The **PDF/CDF** tab presents for each pair of variables the PDF/CDF (default: PDF) of the sample
    together with the distribution PDF.

    .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_PDF.png
        :align: center

    With the graphic settings below the study tree, the user can configure the plot, choose the pair of
    variables to analyze and choose between PDF/CDF.

    .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_PDF_setting.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

  - The **Kendall** tab:

    .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_Kendall.png
        :align: center

    With the graphic settings below the study tree, the user can configure the plot and choose the pair of
    variables to analyze.

    The user can define:
      - The title of the graph, expected to be a string;

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

  - The **Parameters** tab includes the parameters of the selected copula. For the Gaussian copula, the
    tab displays the Spearman's coefficients.

  .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters.png
      :align: center

  - If an error occured when building a copula with the given sample, '-' is indicated in the
    BIC column, the first two tabs are disabled and the **Parameters** tab displays the error message.

  .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters_ErrorMessage.png
      :align: center

4- Metamodel creation
======================

4-1 Definition
''''''''''''''

A new metamodel can be created thanks to:
  - the context menu of a data model **Definition** item;
  - the context menu of a design of experiments item;
  - the **Metamodel creation** box of a model diagram.

The data model or the design of experiments must contain an output sample.

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_wizard.png
    :align: center

Two methods are available:
  - Functional chaos
  - Kriging

The user can choose the outputs to analyze by clicking on the button **-- Select Outputs --**
at the top of the window:

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

By default all the output variables are analyzed.

Functional chaos
~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_functional_chaos_wizard.png
    :align: center

- In the **Parameters** section the user can specify the needed chaos degree. The
  degree must superior or equal to 1. By default, it is equal to 2.

- In the advanced parameters (default: hidden), the user can choose to use a sparse chaos.

Kriging
~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_kriging_wizard.png
    :align: center

- In the **Parameters** section the user can choose:

  - The covariance model (default: Squared exponential) between:
      - Squared exponential,
      - Absolute exponential,
      - Generalized exponential,
      - Matérn model

    To parametrize these models the user can defined:

    - The scale for each input by clicking on the button "**...**" on the
      **Scale** line. A wizard appears with a table containing a column to list
      the input variables and a column to display and edit the scales
      (default: 1.). 

      .. image:: /user_manual/graphical_interface/data_analysis/kriging_scale_wizard.png
          :align: center

    - The amplitude of the process (default: 1., positive float expected).

    - The parameter **p**, exponent of the euclidean norm (default: 1., positive float expected),
      if the **Generalized exponential** model is chosen..

      .. image:: /user_manual/graphical_interface/data_analysis/kriging_p_parameter.png
          :align: center

    - The coefficient **nu** (default: 1.5, positive float expected), if the **Matérn**
      model is chosen.

      .. image:: /user_manual/graphical_interface/data_analysis/kriging_nu_parameter.png
          :align: center

  - The trend basis (default: Constant) between:
      - Constant,
      - Linear,
      - Quadratic

Validation
~~~~~~~~~~

For both analyses it is possible to validate the generated metamodel.
The following page enables to parametrize the validation.
By default the metamodel is validating analytically.

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_validation_page.png
    :align: center

Four methods are available:
  - Analytically. This method correspond to an approximation of the Leave-one-out method result.
      - Kriging: cf O. Dubrule, Cross Validation of Kriging in a Unique Neighborhood,
        Mathematical Geology,1983.
      - Functional chaos: cf G. Blatman, Adaptive sparse polynomial chaos
        expansions for uncertainty propagation and sensitivity analysis.,
        PhD thesis. Blaise Pascal University-Clermont II, France, 2009.
  - Test sample: The data sample is dividing into two sub-samples:
    a training sample (by default 80% of the sample points) and a test sample
    (by default 20% of the sample points).
    That is to say a new metamodel is building with the training sample
    and it is validating with the test sample.
    The points are randomly picked in the data sample (by default the seed is 1).
  - `K-Fold <http://openturns.github.io/openturns/master/theory/meta_modeling/cross_validation.html>`_
  - `Leave-one-out <http://openturns.github.io/openturns/master/theory/meta_modeling/cross_validation.html>`_.
    Be careful, this method is very time consuming.


4-2 Results
'''''''''''

When validating the previous window, a new element appears in the study tree below the
data model item or the design of experiments item.

The context menu of this item contains these actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Convert metamodel into physical model** (Default: disabled): When the analysis
    is finished or stopped successfully, add the metamodel in the study tree;
  - **Remove**: Remove the analysis from the study.

.. image:: /user_manual/graphical_interface/data_analysis/metamodel_contextMenu.png
    :align: center

The item is associated with a window displaying the list of the parameters, a
progress bar and the buttons **Start** (enabled) and **Stop** (disabled).

.. image:: /user_manual/graphical_interface/data_analysis/metaModelWindow.png
    :align: center

Click on the **Start** button launches the analysis. The user can stop the analysis by clicking
on the **Stop** button.

When the analysis is finished or stopped, a result window appears.

Functional chaos
~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_plot.png
    :align: center

The results window gathers:

- The **Metamodel** tab which contains for each output:
    - the relative error: :math:`\displaystyle rel = \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(y_i - \bar{y})^2}}`
    - the residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.

  It contains also a plot opposing the metamodel values to the physical model output values.
  A diagonal is built with the physical model output values.

  The user can configure the plot with the graphic settings below the study tree.

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- The **Summary** tab presents:
    - the first and second order moments;
    - the polynomial basis size, dimension and maximum degree.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_moments.png
      :align: center

- The **Sobol indices** tab contains the first and total order indices plotted for each input variable.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_sobol_indices.png
    :align: center

  The user can configure the plot with the graphic settings below the study tree;

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_sobol_indices_graph_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

  - The tab has a table with the first and total order indices value for each variable. Each
    column can be sorted by clicking on its header. When sorting the table, the
    points on the graphic are also sorted;

  - The index corresponding to the interactions;

  .. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

If the Sobol's indices estimates are incoherent, refer to the warning message in the tooltip of |attentionButton|,
and try to perform the analysis with a greater sample size.

- If the user requested a metamodel validation, the window contains a **Validation** tab.
  This tab presents for each methods and for each output:

    - the metamodel predictivity coefficient: :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)^2}}`
    - the residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.

  It contains also a plot which opposes the predicted metamodel values
  to the physical model output values (except for the analytical validation of
  the metamodel built with the functional chaos method). A diagonal is built
  with the physical model output values.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
      :align: center

  The user can configure the plot with the graphic settings below the study tree:

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_graph_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- The **Parameters** tab which contains the analysis' parameters' values.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_parameters.png
      :align: center


Kriging
~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_plot.png
    :align: center

The results window gathers:

- The **Metamodel** tab which contains a plot opposing the metamodel values to the physical
  model output values. A diagonal is built with the physical model output values.

  The user can configure the plot with the graphic settings below the study tree.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_graph_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- The **Results** tab presents the optimized covariance model parameters and the trend coefficients.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_results.png
      :align: center

- As for the functional chaos result window, if the user requested a metamodel validation,
  the window contains a **Validation** tab.

- The **Parameters** tab which contains the analysis' parameters' values.
