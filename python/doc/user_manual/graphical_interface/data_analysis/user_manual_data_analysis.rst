===========================
User manual - Data analyses
===========================

The OTGui allows the user to launch three types of data analyses:

- Data analyses to get moments, minimum, maximum, PDF, etc;
- Inferences;
- Metamodels

1- Data analysis
================

The right click on a data model proposes to analyse the sample.

At the left of the result window the section **Variables** enables the user to choose the result to display.
The results window gathers 4 tabs:

- The tab **Summary** includes, for a selected variable:
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

.. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_summary.png
    :align: center

- The tab **PDF/CDF** presents the PDF/CDF (default: PDF) of the sample
  together with a kernel smoothing representation.

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

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_PDF.png
      :align: center

- The tab **Box plots** presents the box plot of the sample.

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_boxplot.png
      :align: center

  The user can configure the box plot with the graphic settings (below the study tree).

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

- The tab **Scatter plots** gathers 3 tabs:

  - The tab **Scatter plots** presents the scatter plot of two parameters (Default:
    first output vs first input).

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_scatterplot.png
        :align: center

    The user can configure the scatter plot with the graphic settings (below the study tree).

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_scatterplot_setting.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;
      - The variable plotted on the X-axis, by default the first input;
      - The variable plotted on the Y-axis, by default the first output.

    The scatter plot is plotted in the ranks space when the user checks **Ranks**.

    It has two tabs, **X-axis** and **Y-axis** where the user can define:
      - The axis title, expected to be a string;
      - The minimum bound of the axis, expected to be a floating point;
      - The maximum bound of the axis, expected to be a floating point.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

  - The tab **Plot matrix X-X** gathers:
      - Out of the diagonal the scatter plot of each couple of inputs;
      - The histogram of the distribution of each input on the diagonal.

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixXX.png
        :align: center

    The user can configure the matrix plot with the graphic settings (below the study tree).

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX_setting.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;
      - The variable plotted on the columns, by default all the input variables;
      - The variable plotted on the rows, by default all the output variables.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

  - The tab **Plot matrix Y-X** presents the scatter plot of each output with respect
    to each input in a matrix plot. 

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX.png
        :align: center

    The user can configure the matrix plot with the graphic settings (below the study tree).

    .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX_setting.png
        :align: center

    The user can define:
      - The title of the graph, expected to be a string;
      - The variable plotted on the columns, by default all the input variables;
      - The variable plotted on the rows, by default all the output variables.

    The **Export** button opens a dialog box for the user to select where the
    figure has to be saved.

.. _inferenceAnalysis:

2- Inference analysis
=====================

The right click on a data model proposes to perform an inference analysis.
The inference analysis allows to perform a Kolmogorov goodness-of-fit test for 1-d
continuous distributions.

2-1 Definition
''''''''''''''

.. image:: /user_manual/graphical_interface/data_analysis/inference_wizard.png
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

2-2 Results
'''''''''''

The results window gathers 2 tabs for each variable:

- The tab **Summary** includes, for a selected variable, a table of all the tested
  distributions and the associated p-value. The last column
  indicates whether the distribution is accepted or not according to the given level.
  The distributions are sorted in decreasing order of p-values.
  The right side of this table is updated according to the distribution which is selected.

  This section contains 3 tabs:

  - The tab **PDF/CDF** presents the PDF/CDF (default: PDF) of the sample
    together with the distribution PDF.

  .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_PDF.png
      :align: center

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

  - The tab **Q-Q plot** presents the Q-Q plot which opposes the data quantiles to the quantiles
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


3- Dependencies inference analysis
==================================

The dependencies inference analysis allows to infer the Gaussian copula on the input sample
of the design of experiment.

3-1 Results
'''''''''''

The results window gathers the tab **Summary** for each set of variables.

- The tab **Summary** includes, for a selected set of variables, a table of all the tested
  copulae. The right side of this tab is updated according to the distribution which is selected.

  This section contains 2 tabs:

  - The tab **PDF/CDF** presents for each pair of variables the PDF/CDF (default: PDF) of the sample
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

  - The tab **Parameters** includes the parameters of the selected copula. For the Gaussian copula, the
    tab displays the Spearman's coefficients.

  .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters.png
      :align: center

4- Metamodel creation
======================

4-1 Definition
''''''''''''''

A new metamodel can be created thanks to be context menu of a data model or a
design of experiment. Both must contain an output sample.

Two methods are available:

- Functional chaos
- Kriging

Functional chaos
~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_functional_chaos_wizard.png
    :align: center

The user can choose the output variables of interest of the design of experiment
or data model by clicking on the button **-- Select Outputs --** at the top of the window.
By default all the output variables are analysed.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

- In the section **Parameters** the user can specify the needed chaos degree. The
  degree must superior or equal to 1. By default, it is equal to 2.

- In the section **Validation** the user can request a validation of the metamodel
  by the Leave-one-out method. Be careful, this method is very time consuming.

- In the advanced parameters, the user can choose to use a sparse chaos.

Kriging
~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_kriging_wizard.png
    :align: center

The user can choose the output variables of interest of the design of experiment
or data model by clicking on the button **-- Select Outputs --** at the top of the window.
By default all the output variables are analysed.

.. image:: /user_manual/graphical_interface/probabilistic_analysis/analyses_selectionOutput.png
    :align: center

- In the section **Parameters** the user can choose:

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


4-2 Results
'''''''''''

For the two methods the results window gathers:

- The tab **Metamodel** which contains a plot opposing the metamodel values
  to the physical model output values. A diagonal is built with the physical
  model output values.
  For the functional chaos method, it presents for each output the metamodel predictivity coefficient
  :math:`\displaystyle R2 = \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)}}`
  and the residual :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_plot.png
      :align: center

  The user can configure the plot with the graphic settings below the study tree:

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_metamodel_graph_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- If the user requested a metamodel validation by the Leave-one-out method,
  the window contains a tab **Validation**.
  It presents for each output the metamodel predictivity coefficient
  :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)}}`
  and the residual :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
  It contains also a plot opposing the predicted metamodel values by Leave-one-out
  to the physical model output values. A diagonal is built with the physical
  model output values.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
      :align: center

  The user can configure the plot with the graphic settings below the study tree:

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_metamodel_graph_setting.png
      :align: center

  The user can define:
    - The title of the graph, expected to be a string;

  It has two tabs, **X-axis** and **Y-axis** where the user can define:
    - The axis title, expected to be a string;
    - The minimum bound of the axis, expected to be a floating point;
    - The maximum bound of the axis, expected to be a floating point.

  The **Export** button opens a dialog box for the user to select where the
  figure has to be saved;

- The tab **Parameters** which contains the analysis' parameters' values.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_parameters.png
      :align: center

Functional chaos
~~~~~~~~~~~~~~~~

The results window gathers two tabs:

- The tab **Moments** presents the first and second order moments.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_moments.png
      :align: center

- The tab **Sobol indices** contains the first and total order indices plotted for each input variable.

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

