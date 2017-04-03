=====================================
User manual - Define a physical model
=====================================


The context menu (right click) of the OTStudy or its window provides three ways to define a physical model:

- New symbolic physical model
- New Python physical model
- New YACS physical model

The creation of a physical model adds a physicalModel item in the study tree which gathers three
items:

- Deterministic study
- Probabilistic study
- Designs of experiment

1- "Symbolic physical model": define a physical model with analytical formulas
================================================================================

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

- _e
- _pi


2- "Python physical model": define a physical model with a Python editor
========================================================================

It consists in defining the physical model as a Python function **_exec**

.. image:: /user_manual/graphical_interface/physical_model/pythonPhysicalModel.png
    :align: center

This function takes floating point arguments and returns a sequence of floating points.

Input variables names are detected from the fonction definition.
The value of the input variables can be set by the user and are expected
to be floating points or integers. By default they are equal to 0.

The variables listed in the return sequence are detected as output variables.

Other statements which may be used by the **_exec** function can be defined in the Python editor.

The **Evaluate** button calculates the value of the outputs based on the input values
and the Python code. It shall be used to test the physical model. In order to get evaluations which can be saved,
the user should use :ref:`the deterministic study <deterministicStudy>`.

3- "YACS physical model": define a physical model with an XML file
==================================================================

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

