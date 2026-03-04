%feature("docstring") PERSALYS::DataSensitivityAnalysisResult
"Results of a data sensitivity analysis.

Provides access to first order sensitivity indices, their confidence intervals, and information about input independence."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getFirstOrderSobolIndices
"Get the first order Sobol' indices.

Returns
-------
indices : sequence of :py:class:`openturns.Point`
    First order sensitivity indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getFirstOrderSobolIndicesInterval
"Get the confidence intervals for the first order Sobol' indices.

Returns
-------
intervals : sequence of :py:class:`openturns.Interval`
    Confidence intervals for the first order sensitivity indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getSRCIndices
"Get the Standardized Regression Coefficients (SRC) indices.

Returns
-------
indices : :py:class:`openturns.Sample`
    SRC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getSignedSRCIndices
"Get the signed Standardized Regression Coefficients (SRC) indices. 

Returns
-------
indices : :py:class:`openturns.Sample`
    Signed SRC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getSRCIndicesInterval
"Get the confidence intervals for the Standardized Regression Coefficients (SRC) indices.

Returns
-------
intervals : sequence of :py:class:`openturns.Interval`
    Confidence intervals for the SRC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getSignedSRCIndicesInterval
"Get the confidence intervals for the signed Standardized Regression Coefficients (SRC) indices.

Returns
-------
intervals : sequence of :py:class:`openturns.Interval`
    Confidence intervals for the signed SRC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getR2
"Get the R² values of the linear regression models used to compute SRC indices.

Returns
-------
r2_values : sequence of float
    R² values for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getGlobalHSICIndices
"Get the global HSIC indices.

Returns
-------
indices : sequence of float
    Global HSIC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getGlobalR2HSICIndices
"Get the global R2-HSIC indices.

Returns
-------
indices : sequence of float
    Global R2-HSIC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getGlobalPValuesAsymptotic
"Get the asymptotic p-values for the global HSIC indices.

Returns
-------
p_values : sequence of float
    Asymptotic p-values for the global HSIC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getGlobalPValuesPermutation
"Get the permutation p-values for the global HSIC indices.

Returns
-------
p_values : sequence of float
    Permutation p-values for the global HSIC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::isIndependent
"Performs Spearman test to check if the input variables are independent.

Returns
-------
isIndependent : bool
    if False, the input variables are not independent."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getIndependenceWarningMessage
"Get the warning message if the input variables are not independent.

Returns
-------
message : str
    Warning message about input independence."
