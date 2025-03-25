%feature("docstring") PERSALYS::SobolResult
"Get the results of a Sobol sensitivity analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getInputNames
"Input variables names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Input variables names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getOutputNames
"Output variables names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Output variables names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getFirstOrderIndices
"First order indices accessor.

Returns
-------
indices : :py:class:`openturns.Sample`
    First order indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getTotalIndices
"Total indices accessor.

Returns
-------
indices : :py:class:`openturns.Sample`
    Total indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getFirstOrderIndicesInterval
"First order indices interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    First order indices interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getTotalIndicesInterval
"Total indices interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    Total indices interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getConfidenceIntervalLength
"Confidence interval length accessor.

Returns
-------
length : float
    Maximum confidence interval length of all components and both indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getElapsedTime
"Elapsed time in seconds to perform the analysis.

Returns
-------
time : int
    Elapsed time in seconds to perform the analysis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getCallsNumber
"Number of calls accessor.

Returns
-------
calls : int
    Number of function calls"
