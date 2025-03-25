%feature("docstring") PERSALYS::SRCResult
"Get the results of a SRC sensitivity analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getInputNames
"Input variable names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Input variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getOutputNames
"Output variable names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Output variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCResult::getIndices
"SRC indices accessor.

Returns
-------
indices : :py:class:`openturns.Point`
    SRC indices"
