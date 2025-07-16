%feature("docstring") PERSALYS::DataSensitivityAnalysisResult
"Results of a data sensitivity analysis.

Provides access to first order sensitivity indices, their confidence intervals, and information about input independence.
"

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getFirstOrderIndices
"Get the first order sensitivity indices.

Returns
-------
indices : sequence of :py:class:`openturns.Point`
    First order sensitivity indices for each output.
"

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getFirstOrderIndicesInterval
"Get the confidence intervals for the first order sensitivity indices.

Returns
-------
intervals : sequence of :py:class:`openturns.Interval`
    Confidence intervals for the first order sensitivity indices.
"

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::isIndependent
"Performs Spearman test to check if the input variables are independent.

Returns
-------
isIndependent : bool
    if False, the input variables are not independent.
"

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getIndependenceWarningMessage
"Get the warning message if the input variables are not independent.

Returns
-------
message : str
    Warning message about input independence.
"
