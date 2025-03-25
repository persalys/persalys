%feature("docstring") PERSALYS::Input
"Create an input variable.

Represents an input variable.

Parameters
----------
name : str
    Name
value : float
    Default value
distribution : :py:class:`~openturns.Distribution`
    Associated distribution
description : str
    Description text (optional)

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> R = persalys.Input('R', ot.Normal(75000., 5000.))
>>> S = persalys.Input('S', 10.5)"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::setDistribution
"Distribution accessor.

Parameters
----------
distribution : :py:class:`openturns.Distribution`
    Distribution associated with the variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::getDistribution
"Distribution accessor.

Returns
-------
distribution : :py:class:`openturns.Distribution`
    Distribution associated with the variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::setDistributionParametersType
"Distribution parameters type accessor.

Parameters
----------
parametersType : int
    Distribution parameters index"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::getDistributionParametersType
"Distribution parameters type accessor.

Returns
-------
parametersType : int
    Distribution parameters index"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::setFiniteDifferenceStep
R"RAW(Finite difference step accessor.

Parameters
----------
step : float
    Finite difference step used to define the gradient and the hessian of the model's function.
    By default the step is equal to 1e-7.
    The gradient function is defined with the first order non-centered finite difference
    scheme and the hessian function with the second order centered finite difference scheme.

Notes
-----

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
                                     {4 \epsilon_i \epsilon_j})RAW"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::getFiniteDifferenceStep
"Finite difference step accessor.

Returns
-------
step : float
    Finite difference step used to define the gradient of the model's function"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::isStochastic
"Whether the variable is stochastic.

Returns
-------
isStochastic : bool
    Whether the variable is stochastic"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Input::setStochastic
"Whether the variable is stochastic.

Parameters
----------
isStochastic : bool
    Whether the variable is stochastic"
