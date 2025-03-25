%feature("docstring") PERSALYS::CopulaInferenceSetResult
"Dependence inference result for a set of variables.

"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CopulaInferenceSetResult::getSetOfVariablesNames
"Set of variables names accessor.

Returns
-------
names : :py:class:`openturns.Description`
    Set of variables names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CopulaInferenceSetResult::getTestedDistributions
"Get the sequence of tested distributions.

Returns
-------
distributions : sequence of :py:class:`openturns.Distribution`
    Tested distributions"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CopulaInferenceSetResult::getErrorMessages
"Get the sequence of error messages.

Returns
-------
messages : sequence of :py:class:`openturns.Description`
    Error message for each tested distributions. The error message is empty
    if the distribution has been well tested"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CopulaInferenceSetResult::getKendallPlotData
"Data of the Kendall plot.

Returns
-------
data : 2D sequence of :py:class:`openturns.Sample`
    Data of the Kendall plot for each tested distribution and for each pair
    of variables"
