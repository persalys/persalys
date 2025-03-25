%feature("docstring") PERSALYS::DataAnalysisResult
"Get the results of a data analysis."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getMin
"Minimum accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Minimum"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getMax
"Maximum accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Maximum"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getMean
"Mean accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Mean"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getMedian
"Median accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Median"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getStandardDeviation
"Standard deviation accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Standard deviation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getVariance
"Variance accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Variance"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getCoefficientOfVariation
"Coefficient of variation accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Coefficient of variation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getSkewness
"Skewness accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Skewness"
// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getKurtosis
"Kurtosis accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Kurtosis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getFirstQuartile
"First quartile accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    First quartile"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getThirdQuartile
"Third quartile accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Third quartile"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getMeanConfidenceInterval
"Confidence interval mean accessor.

Parameters
----------
level : float
    Confidence level

Returns
-------
result : :py:class:`openturns.Interval`
    Confidence interval mean"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getStdConfidenceInterval
"Confidence interval standard deviation accessor.

Parameters
----------
level : float
    Confidence level

Returns
-------
result : :py:class:`openturns.Interval`
    Confidence interval standard deviation"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getOutliers
"Outliers accessor.

Returns
-------
result : sequence of :py:class:`openturns.Point`
    Outliers"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getFittedDistribution
"Fitted distribution accessor.

Returns
-------
result : :py:class:`openturns.Distribution`
    Fitted distribution"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getCDF
"CDF accessor.

Returns
-------
result : sequence of :py:class:`openturns.Sample`
    CDF"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getPDF
"PDF accessor.

Returns
-------
result : sequence of :py:class:`openturns.Sample`
    PDF"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getElapsedTime
"Elapsed time in seconds to perform the analysis.

Returns
-------
time : int
    Elapsed time in seconds to perform the analysis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysisResult::getDesignOfExperiment
"Design of experiments accessor.

Returns
-------
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments containing the points really analysed"
