===========================
User manual - Data analyses
===========================

The interface allows to perform the following types of data analyses:
  - Data analyses to get moments, minimum, maximum, PDF, etc.
  - Marginals inferences
  - Dependence inferences
  - Metamodels creation

1- Data analysis
================

1-1 Creation
''''''''''''

New sample analysis can be created through:
  - the context menu of the **Definition** item of the data model

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Data analysis** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisWindow.png
    :align: center

.. _dataanalysisresult:

1-2 Results
'''''''''''

When the analysis is finished or stopped, the below window appears.

.. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_summary.png
    :align: center

The right side of the window contains 4 tabs presenting the analysis of the outputs.
The **Table** tab, the **Cobweb plot** tab, the **Plot matrix** tab and the **Scatter plots** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

- The **Summary** tab summarizes the results of the analysis, for a selected variable (left column):
  sample size, minimum/maximum values, input values at
  the extrema of the outputs, moments estimates and empirical quantiles.

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

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_PDF.png
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

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_boxplot.png
      :align: center

.. _dependenceTab:


- The **Dependence** tab displays the `Spearman's matrix <http://openturns.github.io/openturns/latest/theory/data_analysis/spearman_coefficient.html>`_ estimate.

    - The cells are colored according to the value of the Spearman's coefficient.
    - Its context menu allows to export the table in a CSV file or as a PNG image.
    - Select cells and Press Ctrl+C to copy values in the clipboard

  .. image:: /user_manual/graphical_interface/deterministic_analysis/doe_dependence.png
      :align: center

- The **Table** tab shows the input/output samples. The table can be exported (**Export** button).

  - Interactions:
      - Left-click (optional: + Ctrl) on lines to select them
      - Left-click on column header to sort values in ascending or descending order 
      - Left-click on a column header and drag it in another place to change columns order

  .. image:: /user_manual/graphical_interface/deterministic_analysis/designOfExperimentTable.png
      :align: center


- The **Cobweb plot** tab displays the sample points.

  - The **Graphic settings** window (below the study tree) allows to:
      - Select the variables to display and the axis order
      - Modify the space where the values are plotted with the **Ranks** check button
        (default: physical space). Check off the button to display the rank of the values
      - Export the figure

  - Interaction:
      - Left-click on columns to select curves (multiple selection possible)

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_Cobweb.png
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

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_plotmatrixYX.png
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

  .. image:: /user_manual/graphical_interface/data_analysis/data_model_analysis_scatterplot.png
      :align: center

.. _inferenceAnalysis:

2- Marginals inference
======================

The inference analysis allows to perform a Bayesian Information Criterion
(`BIC <http://openturns.github.io/openturns/latest/theory/data_analysis/bic.html>`_) and
a `Kolmogorov-Smirnov <http://openturns.github.io/openturns/latest/theory/data_analysis/kolmogorov_test.html>`_
goodness-of-fit tests for 1-d continuous distributions.

New marginals inference can be created thanks to:
  - the context menu of the **Definition** item of the data model

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Marginals inference** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _marginalsinferencewizard:

2-1 Definition
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
  - the Kolmogorov-Smirnov level such that :math:`\alpha = 1 - {\rm level}` is the risk of
    committing a Type I error, that is an incorrect rejection of a true null hypothesis
    (default: 0.05., expected: float in the range :math:`]0, 1[`)



2-2 Launch
''''''''''

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has two actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/inferenceWindow.png
    :align: center

.. _marginalsinferenceresult:

2-3 Results
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
      - The **PDF/CDF** tab presents the PDF/CDF (default: PDF) of the sample
        together with the distribution PDF.

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

      - The **Q-Q plot** tab presents the `Q-Q plot <http://openturns.github.io/openturns/latest/theory/data_analysis/graphical_fitting_test.html>`_
        which opposes the data quantiles to the quantiles of the tested distribution.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_QQplot.png
           :align: center

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
                - Graph title (expected: string)
                - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                  string), minimum/maximum bounds (expected: floating point)

        - Interactions:
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

      - The **Parameters** tab includes a table with the moments of the selected distribution
        and the values
        `estimate <http://openturns.github.io/openturns/latest/theory/data_analysis/parametric_estimation.html>`_
        of its native parameters.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters.png
           :align: center

        *failed* in the **Acceptation** column means that an error occured when building a distribution
        with the given sample. Then, the **Parameters** tab shows the error message.

        .. image:: /user_manual/graphical_interface/data_analysis/inference_resultWindow_tab_summary_parameters_error_message.png
            :align: center


The result can be used in the :ref:`Probabilistic model window <inferenceresultwizard>`.

.. _dependenceInference:

3- Dependence inference
=======================

The dependence inference allows to infer copulas on the sample of the data model.

This analysis can be created thanks to:
  - the context menu of the **Definition** item of the relevant data model

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Dependence inference** box of the model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _dependenceinferencewizard:

3-1 Definition
''''''''''''''

When an analysis is required, a window appears:

.. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizard.png
    :align: center

The windows allows to set up:
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

        - *For a pair of variables* : bivariate
          `copulas <http://openturns.github.io/openturns/master/user_manual/_generated/openturns.Copula.html>`_
          are available (Ali-Mikhail-Haq, Clayton, Farlie-Gumbel-Morgenstern, Frank, Gumbel, Normal)
        - *For a group with more than two variables*: only the Normal copula is available
          (**Add** and **Remove** buttons are then disabled)

    .. image:: /user_manual/graphical_interface/data_analysis/dependenceInference_wizard_copulaList.png
      :align: center

To remove a group:
  - Select a group in the second table
  - Click on the left arrow

3-2 Launch
'''''''''''

When the analysis is required, a new item is added in the study tree below the data model item.

Its context menu has the following actions:
  - **Rename**: Rename the analysis;
  - **Modify**: Reopen the setting window to change the analysis parameters;
  - **Remove**: Remove the analysis from the study.

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/copulaInferenceWindow.png
    :align: center

.. _dependenceinferenceresult:

3-3 Results
'''''''''''

When the analysis is finished or stopped, a window appears:

.. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_PDF.png
    :align: center

The window gathers:

- The **Summary** tab includes, for a selected set of variables:
    - a table of all the tested copulas
    - for the selected copula:
        - the **PDF/CDF** tab presents, for each pair of variables, the PDF/CDF (default: PDF) of the sample
          together with the distribution PDF.

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

        - the **Kendall** tab presents for each pair of variables a visual fitting test using the
          `Kendall plot <http://openturns.github.io/openturns/latest/theory/data_analysis/graphical_fitting_test.html>`_.
          This plot can be interprated as for QQ-plot (for marginals): more the curve fit the dialgonal, more
          the dependence model is adequate.

            - The **Graphic settings** window (below the study tree) allows to:
                - Export the figure
                - Set up parameters:
                    - Graph title (expected: string)
                    - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                      string), minimum/maximum bounds (expected: floating point)

            - Interactions:
                - Left-click to translate the graph
                - Mouse wheel up/down to zoom/zoom out

        .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_Kendall.png
            :align: center

        - the **Parameters** tab includes the parameters `estimate <http://openturns.github.io/openturns/latest/theory/data_analysis/parametric_estimation.html>`_ of the selected copula.
            - *For the Gaussian copula*: the tab displays the `Spearman's coefficients <http://openturns.github.io/openturns/latest/theory/data_analysis/spearman_coefficient.html>`_.

            .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters.png
                :align: center

            - *'-'* in the **BIC** column means that an error occured when building a copula
              with the given sample. Then, the **Parameters** tab shows the error message.

            .. image:: /user_manual/graphical_interface/data_analysis/copulaInference_resultWindow_tab_summary_parameters_ErrorMessage.png
                :align: center

The result can be used in the :ref:`Probabilistic model window <dependenceinferenceresultwizard>`.

4- Metamodel creation
======================

To perform this analysis, the data model or the design of experiments must contain an output sample.

A new metamodel can be created thanks to:
  - the context menu of a design of experiments item

    .. image:: /user_manual/graphical_interface/data_analysis/doe_eval_ContextMenu.png
        :align: center

  - the **Metamodel creation** box of a physical model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/physicalModel_Diagram_metamodelBox.png
        :align: center

  - the context menu of the **Definition** item of a data model

    .. image:: /user_manual/graphical_interface/data_analysis/dataAnalysisdefContextMenu.png
        :align: center

  - the **Metamodel creation** box of a data model diagram

    .. image:: /user_manual/graphical_interface/data_analysis/dataModelDiagramBoxes.png
        :align: center

.. _metamodelwizard:

4-1 Definition
''''''''''''''

When an analysis is required, a window appears, in order to set up:
  - the outputs of interest (**Select outputs** - default: all outputs are analyzed)
  - the method: `Functional chaos <http://openturns.github.io/openturns/latest/theory/meta_modeling/functional_chaos.html>`_ (default) or `Kriging <http://openturns.github.io/openturns/latest/theory/meta_modeling/kriging.html>`_

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_wizard.png
    :align: center

Functional chaos
~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_functional_chaos_wizard.png
    :align: center

The «Functional chaos parameters» window allows to define:
  - **Parameters**: chaos degree (default: 2, expected: integer greater or equal to 1)
  - **Advanced Parameters** (default: hidden): sparse chaos (default: not sparse)

Kriging
~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_kriging_wizard.png
    :align: center

The «Kriging parameters» window allows to define:
  - **Parameters**:
     - the covariance model: Squared exponential (default), Absolute exponential,
       Generalized exponential, Matérn model
     - the parameters of covariance models:
         - **Generalized exponential**: parameter **p**,
           exponent of the euclidean norm (default: 1., positive float expected)

         .. image:: /user_manual/graphical_interface/data_analysis/kriging_p_parameter.png
              :align: center

         - **Matérn**: coefficient **nu** (default: 1.5, positive float expected)

         .. image:: /user_manual/graphical_interface/data_analysis/kriging_nu_parameter.png
              :align: center

     - the trend basis type: Constant (default), Linear or Quadratic
  - **Advanced Parameters** (default: hidden) for the covariance models:
     - Require the optimization of the covariance model parameters
     - the scale for each input by clicking on the button "**...**" on the
       **Scale** line. A wizard appears with a table containing a column to list
       the input variables and a column to display and edit the scales
       (default: 1.).

     .. image:: /user_manual/graphical_interface/data_analysis/kriging_scale_wizard.png
          :align: center

     - the amplitude of the process (default: 1., positive float expected)


Validation
~~~~~~~~~~

The generated metamodel can be validated. By default, the metamodel is validating analytically.

The following windows allows to set up the validation. Four methods are available:
  - Analytically: This method correspond to an approximation of the Leave-one-out method result.
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
  - `K-Fold <http://openturns.github.io/openturns/master/theory/meta_modeling/cross_validation.html>`_:
     - Define the number of folds (default: 5, expected: integer greater than 1)
     - The folds are generated randomly (by default the seed is 1)
    
.. image:: /user_manual/graphical_interface/data_analysis/metaModel_validation_page.png
    :align: center

4-2 Results
'''''''''''

When the window is validated, a new element appears in the study tree below the
data model item or the design of experiments item.

The context menu of this item contains these actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Convert metamodel into physical model** (default: disabled, enabled when the analysis
    is successfully finished): Add the metamodel in the study tree
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/data_analysis/metaModelWindow.png
    :align: center

.. _functionalchaosresult:

Functional chaos
~~~~~~~~~~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_plot.png
    :align: center

The results window gathers:

- The **Metamodel** tab which contains for the selected output (left column):
    - the number of points
    - the relative error: :math:`\displaystyle rel = \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(y_i - \bar{y})^2}}`
    - the residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
    - a plot opposing the metamodel values (**Prediction**) to the physical model output values
      (**Real output values**). The diagonal (in black) is built with the physical model output values.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
                - Graph title (expected: string)
                - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                  string), minimum/maximum bounds (expected: floating point)

        - Graph interactions:
           - Left-click to translate the graph
           - Mouse wheel up/down to zoom/zoom out

- The **Results** tab presents, for a selected output (left column):
    - the first and second order moments
    - the polynomial basis dimension, maximum degree, full and truncated size
    - the part of variance explains by each polynom

    .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_moments.png
        :align: center

- The **Sobol indices** tab includes, for a selected output (left column):

  - the graph of the input variables first and total order indices.

    - The **Graphic settings** window (below the study tree) allows to:
       - Export the figure
       - Set up parameters:
          - Graph title (expected: string)
          - X-axis labels orientation
          - Axes title (expected: string)

  - the table of the input variables first and total order indices.

      - Interactions:
          - Select cells and Press Ctrl+C to copy values in the clipboard
          - Left-click on column header to sort values in ascending or descending order.
            When sorting the table, the indices are also sorted on the graph.

  - The index corresponding to the interactions.

  .. |attentionButton| image:: /user_manual/graphical_interface/probabilistic_analysis/task-attention.png

  If the Sobol's indices estimates are incoherent, refer to the warning message in the tooltip of |attentionButton|,
  and try to perform the analysis with another sample size.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_sobol_indices.png
    :align: center

- The **Validation** tab (if a metamodel validation required) shows for each method and for the selected output:
    - the metamodel predictivity coefficient: :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)^2}}`
    - the residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
    - *K-Fold* and *Test sample*: A plot which opposes the predicted metamodel values
      to the physical model output value. A diagonal is built
      with the physical model output values.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
                - Graph title (expected: string)
                - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                  string), minimum/maximum bounds (expected: floating point)

        - Graph interactions:
           - Left-click to translate the graph
           - Mouse wheel up/down to zoom/zoom out

      .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
         :align: center

    - *Analytical*: the Q2 value

      .. image:: /user_manual/graphical_interface/data_analysis/FC_analyticalValidation.png
         :align: center


- The **Parameters** tab which contains the analysis' parameters' values.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_parameters.png
      :align: center

.. _krigingresult:

Kriging
~~~~~~~

.. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_plot.png
    :align: center

The results window gathers:

- The **Metamodel** tab which contains for the selected output (left column)
  a plot opposing the metamodel values (**Prediction**) to the physical model output values
  (**Real output values**). The diagonal (in black) is built with the physical model output values.

  - The **Graphic settings** window (below the study tree) allows to:
      - Export the figure
      - Set up parameters:
          - Graph title (expected: string)
          - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
            string), minimum/maximum bounds (expected: floating point)

      - Graph interactions:
         - Left-click to translate the graph
         - Mouse wheel up/down to zoom/zoom out

- The **Results** tab presents the optimized covariance model parameters and the trend coefficients.

  .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_kriging_results.png
      :align: center

- If a metamodel validation is required, the window contains a **Validation** tab which shows for each method and for the selected output:
    - the residual: :math:`\displaystyle res = \frac{\sqrt{\sum_{i=0}^N (y_i - \hat{y_i})^2}}{N}`.
    - the metamodel predictivity coefficient: :math:`\displaystyle Q2 = 1 - \frac{\sum_{i=0}^N (y_i - \hat{y_i})^2}{\sum_{i=0}^N {(\bar{y} - y_i)^2}}`
    - a plot which opposes the predicted metamodel values
      to the physical model output value. A diagonal is built
      with the physical model output values.

      - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
                - Graph title (expected: string)
                - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                  string), minimum/maximum bounds (expected: floating point)

      - Graph interactions:
           - Left-click to translate the graph
           - Mouse wheel up/down to zoom/zoom out

      .. image:: /user_manual/graphical_interface/data_analysis/metaModel_result_window_LOO_plot.png
         :align: center

- The **Parameters** tab which contains the analysis' parameters' values.

