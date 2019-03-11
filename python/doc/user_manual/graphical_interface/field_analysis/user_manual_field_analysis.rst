============================
User manual - Field analyses
============================

The interface allows to launch the following types of field analysis:
  - Model evaluations
  - Central tendency analysis

.. _fieldModelEval:

1- Model evaluation
===================

The user may evaluate the physical model for defined input values.

Create a new model evaluation through:
  - The **Definition** item in the context menu of the relevant model in the study tree

    .. image:: /user_manual/graphical_interface/field_analysis/physicalModelDefinitionContextMenu.png
        :align: center

  - The **Model evaluation** box of the model diagram

    .. image:: /user_manual/graphical_interface/field_analysis/model_diagram.png
        :align: center
 
  - The **New evaluation** item in the context menu of the **Evaluation** section

    .. image:: /user_manual/graphical_interface/field_analysis/evaluation_contextMenu.png
        :align: center

.. _fieldmodelevalwizard:

1-1 Definition
''''''''''''''

When this analysis is required, the following window appears to set its parameters.

.. image:: /user_manual/graphical_interface/deterministic_analysis/modelEvaluation.png
    :align: center


When an evaluation is required, a window appears, in order to set up:
  - The outputs of interest (**Select outputs** - default: all outputs are evaluated).
    At least one variable must be selected to validate the window.
  - The value of the input variables (default: value in the physical model definition).
    The values are expected to be floating points or integer.

1-2 Launch
'''''''''''

When the «Model Evaluation» window is validated, a new element appears in the study
tree, below **Evaluation**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/field_analysis/evaluationWindow.png
    :align: center

.. _fieldmodelevalresult:

1-3 Results
'''''''''''

When the evaluation is finished, a result window appears.
This window gathers the following tabs:

- The **Result** tab shows for the selected output (left column):

    - the **Trajectory** tab shows the output values versus the mesh vertices.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Right-click to select the points of trajectories
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/eval_window_trajectory.png
        :align: center

    - The **Table** tab presents the resulting output values for each mesh nodes.
      The table can be exported (**Export** button).

      - Interactions:
          - Left-click (optional: + Ctrl) on lines to select them
          - Left-click on column header to sort values in ascending or descending order 
          - Left-click on a column header and drag it in another place to change columns order

      .. image:: /user_manual/graphical_interface/field_analysis/eval_window_table_traj.png
          :align: center

- The **Input** tab shows:

    - The **Table** tab reminds the input point. The table can be exported (**Export** button).

      - Interactions:
          - Left-click (optional: + Ctrl) on lines to select them
          - Left-click on column header to sort values in ascending or descending order 
          - Left-click on a column header and drag it in another place to change columns order

      .. image:: /user_manual/graphical_interface/field_analysis/eval_window_table_input.png
          :align: center

- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

- The **Model** tab shows the model content used to perform the analysis.


2- Central tendency analysis
============================

Only the Monte Carlo sampling method is available.
In order to perform a central tendency analysis, build first a :ref:`probabilistic model <probaModel>`.

New central tendency analysis can be created thanks to:
  - the context menu of the probabilistic model item

    .. image:: /user_manual/graphical_interface/field_analysis/probabilisticModelContextMenu.png
        :align: center

  - the **Central tendency** box of the model diagram

    .. image:: /user_manual/graphical_interface/field_analysis/model_diagram.png
        :align: center

  - the context menu of the **Central tendency** item (if it already exists)

.. _fieldcentraltendencywizard:

2-1 Definition
''''''''''''''

When an analysis is required, a window appears, in order to set up:
  - the outputs of interest (**Select outputs** - default: all outputs are analyzed)
  - the stopping criteria of the algorithm (Select at least one criterion to validate the window):
      - the maximum computation time (default: one minute)
        (d means days, h means hours, m means minutes, s means seconds)
      - the maximum sampling size (default: 10000, expected: integer)
  - **Evaluation parameter**: the block size – the number of runs launched simultaneously – for parallelization
    purposes (default=1; expected= integer). It must be less than the maximum sampling size.
  - **Advanced Parameters** (default: hidden):
      - the `Karhunen-Loeve <openturns.github.io/openturns/latest/user_manual/_generated/openturns.KarhunenLoeveAlgorithm.html>`_
        threshold (default: 1e-5, expected: float)
      - the seed of the random generator (default: 0, positive integer expected)


.. image:: /user_manual/graphical_interface/field_analysis/MC_wizard.png
    :align: center


2-2 Launch
'''''''''''

When the «Central tendency» window is validated, a new element appears in the study
tree, below **Central tendency**.

Its context menu has the following actions:
  - **Rename**: Rename the analysis
  - **Modify**: Reopen the setting window to change the analysis parameters
  - **Remove**: Remove the analysis from the study

This item is associated with a window displaying the list of the parameters, a
progress bar and Run/Stop buttons, to launch or stop the analysis.

.. image:: /user_manual/graphical_interface/field_analysis/MC_window.png
    :align: center

.. _fieldmontecarloresult:

2-3 Results
'''''''''''

When the analysis is finished or stopped, a result window appears.

The **Trajectories** tab, the **Result/Table** tab, the **Bag chart** tab, the **Functional bag chart** tab,
the **Input/Table** tab and the **Plot matrix** tab
are linked. It means that when the user select some points on one of these representations,
these points are automatically selected on the others.

The results window gathers the following tabs:

- The **Result** tab shows for the selected output (left column):

    - the **Trajectories** tab shows for each input point, the output values versus the mesh vertices.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Select the trajectories to display
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Right-click to select the points of trajectories
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/MC_window_trajectories.png
        :align: center

    - The **Table** tab presents the resulting output values for each input values (lines) and each mesh nodes (columns).
      The table can be exported (**Export** button).

      - Interactions:
          - Left-click (optional: + Ctrl) on lines to select them
          - Left-click on column header to sort values in ascending or descending order 
          - Left-click on a column header and drag it in another place to change columns order

      .. image:: /user_manual/graphical_interface/field_analysis/MC_window_table_traj.png
          :align: center

    - the **Bag chart** tab shows the explained variance (PC2 (=second principal component) versus PC1 (=first principal component)).

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point)

        - Interactions:
            - Right-click to select points
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/MC_window_bagChart.png
        :align: center

    - the **Functional bag chart** tab shows the quantiles (50% and 95%) .

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)

        - Interactions:
            - Right-click to select trajectories
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/MC_window_FunctionalBagChart.png
        :align: center

    - the **Mean trajectory** tab shows the mean and quantiles (5% and 95%) of the trajectories.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Select the trajectories to display
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/MC_window_meanTrajectories.png
        :align: center


- The **Input** tab shows:

    - The **Table** tab presents the input sample generated by the Monte Carlo sampling method.
      The table can be exported (**Export** button).

      - Interactions:
          - Left-click (optional: + Ctrl) on lines to select them
          - Left-click on column header to sort values in ascending or descending order 
          - Left-click on a column header and drag it in another place to change columns order

      .. image:: /user_manual/graphical_interface/field_analysis/MC_window_table_input.png
          :align: center

    - The **Plot matrix** tab: histograms of the distribution of each variable (diagonal) and
      scatter plots between each couple of input variables (off-diagonal).

      - The **Graphic settings** window (below the study tree) allows to:
          - Select the variables to display and the columns order
          - Export the figure

      - Interactions:
          - Right-click to select points
          - Left-click to translate the graph
          - Mouse wheel up/down to zoom/zoom out

      .. image:: /user_manual/graphical_interface/field_analysis/MC_window_plotMatrix_input.png
          :align: center


- The **Decomposition** tab shows for the selected output (left column), the result of the
  Karhunen-Loeve decomposition:

    - the **Modes** tab shows the modes of the Karhunen Loeve decomposition.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Select the trajectories to display
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Right-click to select the points of trajectories
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

    .. image:: /user_manual/graphical_interface/field_analysis/MC_window_modes.png
        :align: center

    - The **Eigenvalues** tab presents the eigenvalues of the modes and a plot of the cumulative eigenvalue sum.

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Select the trajectories to display
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: color, marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

        .. image:: /user_manual/graphical_interface/field_analysis/MC_window_eigenvalues.png
            :align: center

    - the **ξi** tab shows the projections of the trajectories in the Karhunen-Loeve decomposition:

      - the **PDF/CDF** tab presents the PDF/CDF of the **ξi** sample
        together with the distribution PDF

        - The **Graphic settings** window (below the study tree) allows to:
            - Export the figure
            - Select the graphic type: PDF or CDF (default: PDF)
            - Select the **ξi**
            - Set up parameters:
               - Graph title (expected: string)
               - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
                 string), minimum/maximum bounds (expected: floating point), log
                 scale (check button only available if axis values are positive)
            - Plot style: marker size and style (cross, circle, diamond, square, plus)

        - Interactions:
            - Right-click to select the points of trajectories
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

        .. image:: /user_manual/graphical_interface/field_analysis/MC_window_ksi_pdf.png
            :align: center

      - The **Plot matrix** tab: histograms of the distribution of each variable (diagonal) and
        scatter plots between each couple of **ξi** variables (off-diagonal).

        - The **Graphic settings** window (below the study tree) allows to:
            - Select the variables to display and the columns order
            - Export the figure

        - Interactions:
            - Right-click to select points
            - Left-click to translate the graph
            - Mouse wheel up/down to zoom/zoom out

      .. image:: /user_manual/graphical_interface/field_analysis/MC_window_ksi_plotMatrix.png
          :align: center

- The **Correlation** tab presents the empirical correlation of the output between the nodes of the mesh.

  - The **Graphic settings** window (below the study tree) allows to:
      - Export the figure
      - Set up parameters:
          - Graph title (expected: string)
          - Specific parameters for the axes (**X-axis** and **Y-axis** tab): title (expected:
            string), minimum/maximum bounds (expected: floating point)

  - Interactions:
      - Left-click to translate the graph
      - Mouse wheel up/down to zoom/zoom out

  .. image:: /user_manual/graphical_interface/field_analysis/MC_window_correlation.png
      :align: center


- The **Parameters** tab reminds the user all the parameters values to perform the analysis.

- The **Model** tab shows the model content used to perform the analysis.


