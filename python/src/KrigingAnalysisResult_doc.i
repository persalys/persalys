%feature("docstring") PERSALYS::KrigingAnalysisResult
"Get the results of a GPR analysis.

See Also
--------
KrigingAnalysis
"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysisResult::getGPRResultCollection
"Sequence of kriging results accessor.

Returns
-------
results : sequence of `openturns.GaussianProcessResult`
    The GPR results of all the outputs of interest"

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
