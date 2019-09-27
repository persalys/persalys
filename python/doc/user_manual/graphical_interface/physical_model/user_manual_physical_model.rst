=====================================
User manual - Define a physical model
=====================================

This section introduces the models defined with a vectorial function.

Several types of physical models are available:
  - **Symbolic model**: Physical model defined with analytical formulas
  - **Python model**: Physical model defined with a Python script
  - **YACS model**: Physical model defined with a YACS scheme (optional)
  - **FMI model**: Physical model defined with a FMU file (optional)

1- Creation
===========

There are several ways to create a new model:
  - Select an item in the context menu of the study item

    .. image:: /user_manual/graphical_interface/getting_started/studyItemContextMenu.png
        :align: center

  - Click on a button of the study window

    .. image:: /user_manual/graphical_interface/physical_model/studyWindowButtons.png
        :align: center

The creation of a physical model adds a new element in the study tree, below the **Physical models** section.

Different actions are available through the context menu of the model (by right click):
  - **Rename**: Rename the model
  - **Define the model**: Open a new window to define the model
  - **Duplicate the model**: Create a copy of the model in the study tree
  - **Remove**: Remove the model and all the analyses depending on it

.. image:: /user_manual/graphical_interface/physical_model/physicalModelNameContextMenu.png
    :align: center

.. _vectmodeldiagram:

The item is associated with a **Study workflow** window.

.. image:: /user_manual/graphical_interface/physical_model/physicalModelDiagram.png
    :align: center

This diagram displays the possible actions an user can perform in real time. An action
is active when its box is in dark green and when a previous one is valid. A box is disabled when its box
is in light green, and the previous one is not valid.
When the mouse pointer is hovered over a box, an information message appears at the bottom of
the window in order to specify what sort of actions the box proposes. If the box is disabled the
message indicates why the previous one is not valid.

On the screenshot above, the mouse points over the **Probabilistic model definition** box: here the
action is **Define stochastic input variables and the dependence** (below the main window), but
this action is available only if the user defines at least one input variable in the physical model. So,
here the only option of the user is to complete the model.

2- Definition
=============

To define a physical model:
  - Select **Define the model** in the context menu of the model item

    .. image:: /user_manual/graphical_interface/physical_model/physicalModelNameContextMenu.png
        :align: center

  - Click on the **Model definition** box of the model diagram

    .. image:: /user_manual/graphical_interface/physical_model/modeldefBox.png
        :align: center

When the definition is required, a new item named **Definition** appears in the study tree.

Its context menu proposes these actions:
  - **Probabilistic model**: Create stochastic input variables and dependence
  - **Observations**: Import observations of variables of the model
  - **Design of experiments**: Create a new design of experiments
  - **Evaluation**: Evaluate the model at a point
  - **Screening**: Perform a screening analysis
  - **Optimization**: Perform an optimization

.. image:: /user_manual/graphical_interface/physical_model/physicalModelDefinitionContextMenu.png
    :align: center

The new item **Definition** is associated with a window in the main frame, whose title is the name of
the chosen physical model. In this window, the user can specify the model variables (**Definition** tab)
and the finite difference step of each variable (**Differentiation** tab).

.. _vectsymbolicmodel:

2-1 *Symbolic model*: define a physical model with analytical formulas
-------------------------------------------------------------------------------

.. image:: /user_manual/graphical_interface/physical_model/symbolicPhysicalModel.png
    :align: center

Define variables in the **Definition** tab:
  - the **Inputs** section allows to:
      - Add/remove inputs (**Add**/**Remove** button)
      - Rename variables (default: (X0, X1, ...), expected: string (alphanumeric characters only)) (**Name** column)
      - Describe variables (default: empty string, expected: string (no restriction on
        the label format: floating point, integer, string, etc...)) (**Description** column)
      - Assign a value to the inputs (default: 0, expected: floating points or integers) (**Value** column)
  - the **Outputs** section allows to:
      - Add/remove outputs (**Add**/**Remove** button)
      - Enable/disable one or several variables, by checking off the corresponding line (default: all output lines are checked off).
        To perform the analyses, at least one output must be selected.
      - Rename variables (default: (Y0, Y1, ...), expected: string (alphanumeric characters only)) (**Name** column)
      - Describe variables (default: empty string, expected: string (no restriction on
        the label format: floating point, integer, string, etc...)) (**Description** column)
      - Define the mathematical relationships between the input and output variables (**Formula** column).
        Note that the language used here differs from Python (see corresponding syntax for operators,
        mathematical functions and constants below).
      - Launch the computation of the selected output values based on inputs and formula (**Check model** button).
        It shall be used to test the physical model. To save the output values, the user
        should use a :ref:`Model evaluation <modelEvaluation>`.

.. _AvailableSymbolsModel:

Available operators
~~~~~~~~~~~~~~~~~~~

- = (assignment)
- <= (less or equal)
- >= (greater or equal)
- != (not equal)
- == (equal)
- > (greater than)
- < (less than)
- \+ (addition)
- \- (subtraction)
- \* (multiplication)
- / (division)
- ^ (raise x to the power of y)

Available functions
~~~~~~~~~~~~~~~~~~~

- sin
- cos
- tan
- asin
- acos
- atan
- sinh
- cosh
- tanh
- asinh
- acosh
- atanh
- log2
- log10
- log
- ln
- lngamma
- gamma
- exp
- erf
- erfc
- sqrt
- cbrt
- besselJ0
- besselJ1
- besselY0
- besselY1
- sign
- rint
- abs
- min
- max
- avg
- sum
- floor
- ceil
- trunc
- round

Available constants
~~~~~~~~~~~~~~~~~~~

- e\_
- pi\_


.. _vectpythonmodel:

2-2 *Python model*: define a physical model with a Python editor
-------------------------------------------------------------------------

.. image:: /user_manual/graphical_interface/physical_model/pythonPhysicalModel_default.png
    :align: center

The model definition window contains:
 - the **Python editor** allows to define the model with the function **_exec** which:
    - takes floating point arguments (detected as the inputs of the model) (default: X0)
    - returns a float or a sequence of floats (detected as the outputs of the model) (default: Y0 = X0)
    - can use other statements defined in the Python editor

 - the **Inputs** section allows to:
    - List the inputs detected from the function **_exec**
    - Describe variables (default: empty string, expected: string (no restriction on
      the label format: floating point, integer, string, etc...)) (**Description** column)
    - Assign a value to the inputs (default: 0, expected: floating points or integers) (**Value** column)

 - the **Outputs** section allows to:
    - List the outputs detected from the function **_exec**
    - Enable/disable one or several variables, by checking off the corresponding line (default: all output lines are checked off).
      To perform the analyses, at least one output must be selected.
    - Describe variables (default: empty string, expected: string (no restriction on
      the label format: floating point, integer, string, etc...)) (**Description** column)
    - Require the parallelization of the computations (**Enable multiprocessing** button)
    - Launch the computation of the selected output values based on inputs, and the Python function (**Check model** button).
      It shall be used to test the physical model. To save the output values, the user
      should use a :ref:`Model evaluation <modelEvaluation>`.

Here is an example of a more complex model definition:

.. image:: /user_manual/graphical_interface/physical_model/pythonPhysicalModel.png
    :align: center

Here is an example of an error message transmission, when we tried to compute sqrt(C=-450):

.. image:: /user_manual/graphical_interface/physical_model/pythonPhysicalModel_error.png
    :align: center

.. _vectyacsmodel:

2-3 *YACS model*: define a physical model with an XML file
-------------------------------------------------------------------

A physical model can be defined by loading an XML file, previously generated for example with Salome,
which contains:

- Definitions for the input and output variables;

- Computation parameters;

- Actions to be performed to evaluate the model (for instance, a call to Code_Aster solver)

The **Check model** button computes the output values based on inputs and the YACS scheme. It shall be
used to test the physical model. To save the output values, the user should use the :ref:`Model evaluation <modelEvaluation>`.

.. _vectfmimodel:

2-4 *FMI model*: define a physical model from an FMU file
------------------------------------------------------------------

A physical model can be defined by loading a FMU file, previously generated 
by OpenModelica for example.

.. image:: /user_manual/graphical_interface/physical_model/FMIPhysicalModel1.png
    :align: center

The **Properties** tab allows to select the FMU file and to display its properties.

The **Variables** tab describes the model variables.
The main array shows the list of the variables.
While the variability and causality are read-only FMI attributes,
one may want to change whether how variables are used regarding the physical
model: disabled, input or output in the *I/O* column under the following constraints:

- A variable of causality *Input* cannot be disabled
- A variable of causality *Output* or *Local* cannot be used as input
- A variable of causality *Input* or *Parameter* cannot be used as output

.. image:: /user_manual/graphical_interface/physical_model/FMIPhysicalModel2.png
    :align: center

By default, all the variables appear in the array and some filters allow
to modify the currently listed variables.

The **Check model** button runs the model once.
The output values are displayed in the **Value** column.

.. _DifferentiationTab:

2-5 Differentiation tab
-----------------------

.. image:: /user_manual/graphical_interface/physical_model/differentiation_tab.png
    :align: center

The **Differentiation** tab enables the user to define the finite difference step of each input variable.
By default each step is equal to 1e-7.
These steps are used to set the gradient of the model function with the first order non-centered finite difference scheme
and its hessian with the second order centered finite difference scheme.

First order non-centered finite difference scheme:

.. math::

    \frac{\partial f_j}{\partial x_i} \approx \frac{f_j(x + \epsilon_i) - f_j(x)}
                                                   {\epsilon_i}

Second order centered finite difference scheme:

.. math::

  \frac{\partial^2 f_k}{\partial x_i \partial x_j} \approx
                                     \frac{
                                        f_k(x + \epsilon_i + \epsilon_j) -
                                        f_k(x + \epsilon_i - \epsilon_j) +
                                        f_k(x - \epsilon_i - \epsilon_j) -
                                        f_k(x - \epsilon_i + \epsilon_j)}
                                     {4 \epsilon_i \epsilon_j}

