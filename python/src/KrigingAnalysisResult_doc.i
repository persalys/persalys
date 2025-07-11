%feature("docstring") PERSALYS::KrigingAnalysisResult
"Get the results of a Kriging analysis.

See Also
--------
KrigingAnalysis
"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysisResult::getKrigingResultCollection
"Sequence of kriging results accessor.

Returns
-------
results : sequence of `openturns.KrigingResult`
    The kriging results of all the outputs of interest"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysisResult::getResultForVariable
"Get the kriging result of a variable.

Parameters
----------
variable : str
    Name of the variable

Returns
-------
result : `openturns.KrigingResult`
    The kriging result of the variable"
