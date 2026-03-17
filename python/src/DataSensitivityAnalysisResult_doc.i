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

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getHSICIndices
"Get the HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
indices : sequence of :py:class:`openturns.Point`
    HSIC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getR2HSICIndices
"Get the R2-HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
indices : sequence of :py:class:`openturns.Point`
    R2-HSIC indices for each output."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getPValuesAsymptotic
"Get the asymptotic p-values for the HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global or Target).

Returns
-------
p_values : sequence of :py:class:`openturns.Point`
    Asymptotic p-values for the HSIC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getPValuesPermutation
"Get the permutation p-values for the HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
p_values : sequence of :py:class:`openturns.Point`
    Permutation p-values for the HSIC indices."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::computeHSICPValuesAsymptotic
"Whether asymptotic p-values for HSIC indices of the given type were computed.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
compute : bool
    True if asymptotic p-values were computed, false otherwise."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::computeHSICPValuesPermutation
"Whether permutation p-values for HSIC indices of the given type were computed.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
compute : bool
    True if permutation p-values were computed, false otherwise."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysisResult::getInterestVariables
"Get the variables of interest for the sensitivity analysis.

Returns
-------
interest_variables : sequence of str
    Variables of interest for the sensitivity analysis."

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
