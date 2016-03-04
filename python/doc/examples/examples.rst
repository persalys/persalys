Examples
========

Example 1: Deviation of a cantilever beam
-----------------------------------------

This example has been presented in the ESREL 2007 conference in the paper :
OpenTURNS, an Open source initiative to Treat Uncertainties, Risks'N Statistics
in a structured industrial approach, from A. Dutfoy(EDF R&D),
I. Dutka-Malen(EDF R&D), R. Lebrun (EADS innovation Works) et al.

This example is a simple beam, restrained at one side and stressed by a
concentrated bending load F at the other side.

1- Problem statement
````````````````````

a- Inputs
'''''''''

- Stochastics variables:

====== ======================== ===============================
 Name  Description              Distribution
====== ======================== ===============================
E      Young's modulus          Beta(0.93, 3.2, 2.8e7, 4.8e7)
F      Charge applied           LogNormal(30000., 9000., 15000)
L      Length                   Uniform(250, 260)
I      Section modulus          Beta(2.5, 4., 3.1e2, 4.5e2))
====== ======================== ===============================

The input variables :math:`L` and :math:`I` are dependent. The dependence struture
is modelise by a Normal copula (the only copula available for now in the GUI).
The Spearman correlation matrix of the input random vector :math:`E, F, L, I` is :

.. math ::

    R_{S} = \left(
        \begin{array}{cccc}
        1 & 0 & 0 & 0 \\
        0 & 1 & 0 & 0 \\
        0 & 0 & 1 & -0.2 \\
        0 & 0 & -0.2 & 1
        \end{array} \right)

b- Output
'''''''''

The deviation :math:`y` of the free end of the beam equal to :

.. math::

    y = \frac{FL^3}{3EI}

2- Define the model with the OTGui
``````````````````````````````````

2-1 Create the study
''''''''''''''''''''

.. |newButton| image:: /user_manual/graphical_interface/getting_started/document-new22x22.png

There are three ways to create a new study:

- Use the dialog box which appears when the OTGui is launched;

- Click on |newButton| in the tool bar;

- Click on **New OTStudy** in the file menu.

.. image:: new_study.png
    :align: center


2-1 Create the analytical physical model
''''''''''''''''''''''''''''''''''''''''

To define the physical model, choose **New analytical physical model** in the
contextual menu (right click) of the OTStudy in the study tree.

.. image:: contextual_menu_otstudy.png
    :align: center

We obtain the following window :

.. image:: new_analyticalPhysicalModel.png
    :align: center

The creation of a physical model adds a physicalModel item in the study tree which gathers three
items:

- Deterministic study
- Probabilistic study
- Designs of experiment

.. image:: items_of_physicalModel.png
    :align: center

Use the **Add** buttons below the tables to add as many lines as number of variables
in the physical model.

.. image:: add_variables_in_tables.png
    :align: center

Fill the tables and rename variables to correspond to the physical model.

.. image:: good_defined_physicalModel.png
    :align: center

Click on the **Evaluate** button below the output variables table in order to check
if the formula is not badly defined.

.. image:: evaluation_output_PM_window.png
    :align: center

The value of the output :math:`y` must be :math:`13.0208` if :

======= ======= ======= =======
E       F       L       I
======= ======= ======= =======
3e7     3e4     250     400
======= ======= ======= =======


2-1 Create the probabilistic model
''''''''''''''''''''''''''''''''''

To define the probabilistic model, choose **New probabilistic model** in the
contextual menu (right click) of the probabilistic study item in the study tree.
Only one probabilistic model can be defined by physical model.

.. image:: contextual_menu_proba_study.png
    :align: center

The probabilistic model is defined by associated a distribution to input variables
and specify correlation between them if needed.

.. image:: proba_model_default.png
    :align: center

The first tab **Marginals** list automatically all the input variables defined in the physical
model window. By default all the lines are unchecked which means all the inputs
variables are deterministic. In the right side of the window the variable value
is the value mentioned in the physical model window.

To make a variable stochastic, check its line. Then the combobox in the second
column of the table is available and the current text is **Normal**.
By default the Normal distribution is used with a mean value :math:`E[Input]`
equal to the value defined in the physical model window and with a standard
deviation equal to :math:`0.1 |E[Input]|`

.. image:: proba_model_default_distribution.png
    :align: center

Check all the lines and choose the good distribution in the list and change
the parameters values in the right side of the window.

.. image:: proba_model.png
    :align: center

The second tab **Correlation** show the Spearman correlation matrix.
OTGui handles automatically the symmetry of the correlation matrix. The Gaussian
copula is used to model the dependency. By default all variables are set
independent.
To specify the correlation between the variables :math:`L` and :math:`I`
change the value in the corresponding case.

.. image:: correlation.png
    :align: center


3- Central tendency analysis
````````````````````````````

3-1 Taylor Expansions
'''''''''''''''''''''

3-1-1 Inputs
************

To realize a central tendency analysis with the Taylor Expansions method,
choose **New central tendency** in the
contextual menu (right click) of the probabilistic model item in the study tree.

.. image:: contextual_menu_proba_model.png
    :align: center

Check the radio button **Taylor expansions** in the wizard which appears.

.. image:: central_tendency_wizard_Taylor.png
    :align: center

Click on **Done** button. A new item with a default name appears in the study
tree and a results window is created.

3-1-2 Results
*************

The results window contains a table.

.. image:: taylor_results_table.png
    :align: center

3-1 Monte Carlo
'''''''''''''''

To realize a central tendency analysis with the Monte Carlo method,
choose **New central tendency** in the
contextual menu (right click) of the probabilistic model item in the study tree.

.. image:: contextual_menu_proba_model.png
    :align: center

Check the radio button **Monte Carlo** in the wizard which appears.

.. image:: central_tendency_wizard.png
    :align: center

To see advanced parameters, collapse the **Advanced parameters** group.

.. image:: central_tendency_MC_parameters.png
    :align: center

Click on **Done** button. A new item with a default name appears in the study
tree and a results window is created.

3-1 Results
'''''''''''

The results window contains 7 tabs. When requiring 10000 simulations, the seed
number equal to 0 and the computation of the confidence interval of the mean and
the standard deviation at 0.95%, the second tab must be contain the following
values :

.. image:: MonteCarlo_results_window.png
    :align: center


4- Min/Max study with deterministic design of experiments
`````````````````````````````````````````````````````````

3-1 Inputs
''''''''''

To realize a Min/Max study, choose **New design of experiment** in the
contextual menu (right click) of the designs of experiment item in the study tree.

.. image:: contextual_menu_DOE.png
    :align: center

Check the radio button **Deterministic** in the wizard which appears and click on
**Continue** button.

.. image:: DOE_wizard.png
    :align: center

After clicking a table appears.

.. image:: deterministic_design_of_experiment.png
    :align: center

By default all the lines are checked which all the inputs are variable.
The minimum and the maximum values are computed automatically thanks to
the distribution of the variables. The minimum value is the quantile at the
probability of 0.05 and the maximum one is the quantile at the probability of
0.95. The number of used values per variable is by default 2.
Uncheck a line consist in making the variable constant equal to the value set in
the column **Value**.

Click on **Done** button. A new item with a default name appears in the study
tree and a results window is created.

3-2 Results
'''''''''''

3-2-1 Input variables
*********************

The results window contains the design of experiment.

.. image:: DOE_inputs.png
    :align: center

The points are generated according to the structure of a box design of experiments.
This deterministic design of experiments has 8 points obtained by regularly discretizing
the pavement
:math:`[28350160, 42414817] \times [20166.601, 47021.278] \times [250.5, 259.5] \times [342.97477, 441.26225]`.

Click on **Evaluate** button to compute the output variables.

3-2-1 Min/Max values
********************

When the computation is finished, columns corresponding to the output variables
values are added in the table and other tabs appear in the window.

.. image:: DOE_results.png
    :align: center

The second tab must be contain the following values :

.. image:: min_max_values_DOE.png
    :align: center








