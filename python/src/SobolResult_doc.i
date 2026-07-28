%feature("docstring") PERSALYS::SobolResult
"Get the results of a Sobol sensitivity analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getOutputNames
"Output variables names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Output variables names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getInputNames
"Input variables names accessor.

Returns
-------
sample : :py:class:`openturns.Description`
    Input variables names"

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
"First order indices confidence interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    First order indices confidence interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getTotalIndicesInterval
"Total indices confidence interval accessor.

Returns
-------
interval : sequence of :py:class:`openturns.Interval`
    Total indices confidence interval"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getAggregatedFirstOrderIndices
"Aggregated first order indices accessor.

Returns
-------
indices : :py:class:`openturns.Point`
    Aggregated first order indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getAggregatedTotalIndices
"Aggregated total indices accessor.

Returns
-------
indices : :py:class:`openturns.Point`
    Aggregated total indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getAggregatedFirstOrderIndicesInterval
"Aggregated first order indices confidence interval accessor.

Returns
-------
interval : :py:class:`openturns.Interval`
    Aggregated first order indices confidence interval"


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getAggregatedTotalIndicesInterval
"Aggregated total indices confidence interval accessor.

Returns
-------
interval : :py:class:`openturns.Interval`
    Aggregated total indices confidence interval"


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getConfidenceIntervalLength
"Confidence interval length accessor.

Returns
-------
length : float
    Maximum confidence interval length of all components and both indices"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getCallsNumber
"Number of calls accessor.

Returns
-------
calls : int
    Number of function calls"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolResult::getElapsedTime
"Elapsed time in seconds to perform the analysis.

Returns
-------
time : int
    Elapsed time in seconds to perform the analysis"
