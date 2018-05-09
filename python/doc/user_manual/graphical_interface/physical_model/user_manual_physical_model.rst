=====================================
User manual - Define a physical model
=====================================

1- Creation
===========

A physical model can be created through:
  - the context menu of the study item;
  - a button of the study window.

The context menu of the OTStudy or its window provides three types of physical model:
  - Symbolic model
  - Python model
  - YACS model

The creation of a physical model adds a new element in the study tree below an item named **Physical models**.

Its context menu has the following actions:
  - **Rename**: Rename the model;
  - **Define the model**: Open a new window to define the model;
  - **Remove**: Remove the model from the study.

.. image:: /user_manual/graphical_interface/physical_model/physicalModelNameContextMenu.png
    :align: center

The item is associated with a 'Physical model diagram' window containing an **Use pattern**.

.. image:: /user_manual/graphical_interface/physical_model/physicalModelDiagram.png
    :align: center

This diagram displays the possible actions that the user can perform in real time.
An action is active when its box is in dark green and disabled when its box is in light green.
A box is active when the previous one is valid.
When passing the cursor on each box, an information message appears at the bottom of the window
in order to specify what sort of actions the box proposes. If the box is disabled the message
indicates why the previous one is not valid.

On the above screenshot, the cursor is on the **Probabilistic model definition** box.
The text explains that this box enables to define stochastic input variables and dependence,
but these actions are possible only if the user has defined at least one input variable in the
physical model.
So, here the only option of the user is to complete the model.

2- Definition
=============

New physical model can be defined through:
  - the context menu of the physical model item;
  - the **Model definition** box of the model diagram.

When the definition is requiring, a new item named **Definition** appears in the study tree.

Its context menu proposes these actions:
  - **Probabilistic model**: Create stochastic input variables and dependence
  - **Evaluation**: Evaluate the model at a point
  - **Design of experiments**: Create a new design of experiments

.. image:: /user_manual/graphical_interface/physical_model/physicalModelDefinitionContextMenu.png
    :align: center

This item is associated with a window depending of the type of the chosen physical model.

2-1 "Symbolic physical model": define a physical model with analytical formulas
-------------------------------------------------------------------------------

It consists in listing manually all the input and output variables by adding
lines in the two tables "Inputs" and "Outputs".
By default the inputs are deterministic variables equal to 0. Values are expected
to be floating points or integers.

.. image:: /user_manual/graphical_interface/physical_model/symbolicPhysicalModel.png
    :align: center

Names of the variables are strings. Only alphanumeric characters are allowed. Default set the name of the variable
to X0 for the first input, X1 for the second, Y0 for the first output, etc...
A label can be assigned to each input and output variable in the **Description** column. There is no restriction
on the label value (floating point, integer, string, etc...).

Each line of the "Outputs" table is checkable to allow the user to enable or disable one or several variables.
By default all of the outputs are checked. The model must have at least one selected output to perform
analyses.

The relationships between the inputs and the outputs have to be defined in the **Formula** column.
The language used here is different from Python. Syntaxes for the operators, mathematical functions
and constants are listed below.

The **Evaluate** button computes the value of the outputs based on the input values
and the formula. It shall be used to test the physical model. In order to get evaluations which can be saved,
the user should use :ref:`the Deterministic Study <deterministicStudy>`.

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


2-2 "Python physical model": define a physical model with a Python editor
-------------------------------------------------------------------------

It consists in defining the physical model as a Python function **_exec**

.. image:: /user_manual/graphical_interface/physical_model/pythonPhysicalModel.png
    :align: center

This function takes floating point arguments and returns a float or a sequence of floats.

Input variables names are detected from the function definition.
The value of the input variables can be set by the user and are expected
to be floating points or integers. By default they are equal to 0.

The variables listed in the return are detected as output variables.

Other statements which may be used by the **_exec** function can be defined in the Python editor.

The **Evaluate** button calculates the value of the outputs based on the input values
and the Python code. It shall be used to test the physical model. In order to get evaluations which can be saved,
the user should use :ref:`the deterministic study <deterministicStudy>`.

2-3 "YACS physical model": define a physical model with an XML file
-------------------------------------------------------------------

A physical model can be defined by loading an XML file, previously generated for example with Salome,
which contains:

- Definitions for the input and output variables;

- Computation parameters;

- Actions to be performed to evaluate the model (for instance, a call to Code_Aster solver)

.. image:: /user_manual/graphical_interface/physical_model/YACSPhysicalModel.png
    :align: center

The user requires the parallelization of the computation when checking the check box
**Parallelize status**.
Because of a temporary problem with the module of Code_Aster, it is necessary to
uncheck **Parallelize status** before launching analyses using Code_Aster solver.

The combo box **Fitting machines** lists all the machines defined in the xml file where the computation
can be realized.

The **Evaluate** button calculates the value of the outputs based on the input values
and the YACS scheme. It shall be used to test the physical model. In order to get evaluations which can be saved,
the user should use :ref:`the deterministic study <deterministicStudy>`.

2-4 "FMI physical model": define a physical model from an FMU file
------------------------------------------------------------------

A physical model can be defined by loading an FMU file, previously generated 
by OpenModelica for example.

The first tab allows to select the fmu file and display some properties of the
file.

.. image:: /user_manual/graphical_interface/physical_model/FMIPhysicalModel1.png
    :align: center

Then a second tab is open and describes the model variables.
The variables exposed by the model are listed in the main array.
While the variability and causality are read-only FMI attributes,
one may want to change whether how variables are used regarding the physical
model: disabled, input or output in the *I/O* column under these constraints:

- A variable of causality *Input* cannot be disabled
- A variable of causality *Output* or *Local* cannot be used as input
- A variable of causality *Input* or *Parameter* cannot be used as output

.. image:: /user_manual/graphical_interface/physical_model/FMIPhysicalModel2.png
    :align: center

By default all the variables appear in the array and some filters allow
to alter the currently listed variables.

The **Evaluate** runs the model once.
The output values are displayed in the *value* column.

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

