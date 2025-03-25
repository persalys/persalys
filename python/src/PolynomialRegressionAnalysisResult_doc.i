%feature("docstring") PERSALYS::PolynomialRegressionAnalysisResult
"Get the results of a Linear model analysis.

Notes
-----
Structure created by the method run() of
:class:`~persalys.PolynomialRegressionAnalysis`, and obtained thanks to the method
getResult()."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::PolynomialRegressionAnalysisResult::getLinearModelResultCollection
"Sequence of linear model results accessor.

Returns
-------
results : Collection of :class:`openturns.LinearModelResult`
    The linear model results of all the outputs of interest"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::PolynomialRegressionAnalysisResult::getResultForVariable
"Get the linear model result of a variable.

Parameters
----------
variable : str
    Name of the variable

Returns
-------
result : py:class:`openturns.LinearModelResult`
    The linear model result of the variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::PolynomialRegressionAnalysisResult::getFormulas
"Accessor to the regression formulas.

Returns
-------
formulas : py:class:`openturns.Description`
    The list of formulas"
