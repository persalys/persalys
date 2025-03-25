%feature("docstring") PERSALYS::InferenceResult
"Get the results of an inference analysis.

Notes
-----
Structure created by the method run() of
:class:`~persalys.InferenceAnalysis`, and obtained thanks to the method
getResult()."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceResult::getFittingTestResultCollection
"Sequence of fitting test results accessor.

Returns
-------
results : sequence of :class:`~persalys.FittingTestResult`
    The fitting test results of all the variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceResult::getFittingTestResultForVariable
"Get the fitting test result of a variable.

Parameters
----------
variable : str
    Name of the variable

Returns
-------
result : :class:`~persalys.FittingTestResult`
    The fitting test result of the variable"
