%feature("docstring") PERSALYS::SRCResult
"Get the results of a SRC sensitivity analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getOutputNames
"Output variable names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Output variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getInputNames
"Input variable names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Input variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getIndices
"SRC indices accessor.

Returns
-------
indices : :py:class:`openturns.Point`
    SRC indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getSignedIndices
"Signed SRC indices accessor.

Returns
-------
indices : :py:class:`openturns.Point`
    Signed SRC indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getIndicesInterval
"SRC indices confidence interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    SRC indices confidence interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getSignedIndicesInterval
"Signed SRC indices confidence interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    Signed SRC indices confidence interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getCallsNumber
"Number of calls accessor.

Returns
-------
calls : int
    Number of function calls"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getR2
"R² coefficient accessor.

Returns
-------
r2 : :py:class:`openturns.Point`
    R² coefficient for each output variable"
