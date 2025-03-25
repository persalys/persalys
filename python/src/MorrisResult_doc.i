%feature("docstring") PERSALYS::MorrisResult
"Get the result of a Morris analysis.

Notes
-----
Structure created by the method run() of
:class:`~persalys.MorrisAnalysis`, and obtained thanks to the method
getResult()."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisResult::getInputsSelection
"Input selection accessor.

Parameters
----------
marginal : int
    Output marginal of interest

Returns
-------
selection : :py:class:`openturns.Indices`
    Whether the inputs are selected"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisResult::setInputsSelection
"Input selection accessor.

Parameters
----------
marginal : positive int
    Output marginal of interest
selection : :py:class:`openturns.Indices`
    Whether the inputs are selected"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisResult::getNoEffectBoundary
"No effect boundary accessor.

Parameters
----------
marginal : int
    Output marginal of interest

Returns
-------
boundary : float
    No effect boundary"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisResult::setNoEffectBoundary
"No effect boundary accessor.

Parameters
----------
marginal : positive int
    Output marginal of interest
boundary : float
    No effect boundary"
