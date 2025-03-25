%feature("docstring") PERSALYS::CalibrationAnalysisResult
"Get the results of a calibration analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getCalibrationResult
"Calibration result accessor.

Returns
-------
result : :py:class:`openturns.CalibrationResult`
    Calibration result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getConfidenceInterval
"Confidence interval of the posterior distribution accessor.

Returns
-------
interval : :py:class:`openturns.Interval`
    Confidence interval of the posterior distribution"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getOutputAtPrior
"Output values at prior point accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output values evaluated at prior point (mean of the prior distribution)"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getOutputAtPosterior
"Output values at posterior point accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output values evaluated at calibrated point (mean of the posterior distribution)"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getPriorResidualsPDF
"Prior output residuals PDF accessor.

Returns
-------
samples : sequence of :py:class:`openturns.Sample`
    For each output, the Probability Distribution Function values of the difference
    between the output observations and the prior output values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysisResult::getPosteriorResidualsPDF
"Posterior output residuals PDF accessor.

Returns
-------
samples : sequence of :py:class:`openturns.Sample`
    For each output, the Probability Distribution Function values of the difference
    between the output observations and the posterior output values"

