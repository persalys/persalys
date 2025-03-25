%feature("docstring") PERSALYS::QuantileAnalysis
"Create a data analysis of a design of experiments.

Parameters
----------
name : str
    Name
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> ot.RandomGenerator.SetSeed(0)

Create the model:

>>> filename = 'data.csv'
>>> sample = ot.Normal(3).getSample(100)
>>> sample.exportToCSVFile(filename)
>>> model = persalys.DataModel('myDataModel', 'data.csv', [0, 1, 2])

Create the Quantile analysis:

>>> analysis = persalys.QuantileAnalysis('analysis', model)
>>> analysis.setTargetProbabilities([[1e-2]]*3)
>>> analysis.setTailTypes([persalys.QuantileAnalysisResult.Upper]*3)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getParameterSampleSize
"Parameters sample size accessor.

Returns
-------

size : int
    Number of Generalized Pareto set of parameters used for estimating quantiles confidence interval. It is set by default to 1000."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setParameterSampleSize
"Parameters sample size accessor.

Parameters
----------

size : int
    Number of Generalized Pareto set of parameters used for estimating quantiles confidence interval."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getThreshold
"Threshold accessor.

Returns
-------

threshold : :py:class:`openturns.Sample`
    Threshold sample, dimension: number of marginals, size=2. threshold[0]: lower threshold, threshold[1]: upper threshold."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setThreshold
"Threshold accessor.

Parameters
----------

threshold : :py:class:`openturns.Sample`
    Threshold sample, dimension: number of marginals, size=2. threshold[0]: lower threshold, threshold[1]: upper threshold."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getCDFThreshold
"CDF threshold accessor.

Returns
-------

cdfThreshold : :py:class:`openturns.Sample`
    CDF threshold sample, dimension: number of marginals, size=2. threshold[0]: lower threshold, threshold[1]: upper threshold."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getConfidenceIntervalLevel
"Confidence level accessor.

Returns
-------

ciLevel : double
    Confidence level for quantile intervals."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setConfidenceIntervalLevel
"Confidence level accessor.

Parameters
----------

ciLevel : double
    Confidence level for quantile intervals."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getResult
"Result accessor.

Returns
-------

result : :py:class:`persalys.QuantileAnalysisResult`
    Analysis result."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getSeed
"Random generator seed.

Returns
-------

seed : int
    Random generator seed."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setSeed
"Random generator seed.

Parameters
----------

seed : int
    Random generator seed."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getTargetProbabilities
"Target probabilities accessor.

Returns
-------

targetProba : :py:class:`openturns.PointCollection`
    Collection (size: number of marginals) of points (dimension: number of terget probabilities for each marginals) for which the quantiles are estimated."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setTargetProbabilities
"Target probabilities accessor.

Parameters
----------

targetProba : :py:class:`openturns.PointCollection`
    Collection (size: number of marginals) of points (dimension: number of terget probabilities for each marginals) for which the quantiles are estimated."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getType
"Analysis type accessor

Returns
-------

type : enum
     Analysis type, either `persalys.QuantileAnalysisResult.MonteCarlo` or `persalys.QuantileAnalysisResult.GeneralizedPareto`."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setType
"Analysis type accessor

Parameters
----------

type : enum
     Analysis type, either `persalys.QuantileAnalysisResult.MonteCarlo` or `persalys.QuantileAnalysisResult.GeneralizedPareto`."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::getTailTypes
"Tail types accessor.

Returns
-------

types : :py:class:`openturns.Indices`
    Collection of bit-wise tail type for each marginal. Lower = 1, Upper = 2, Bilateral = 4"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::QuantileAnalysis::setTailTypes
"Tail types accessor.

Parameters
----------

types : :py:class:`openturns.Indices`
    Collection of bit-wise tail type for each marginal. Lower = 1, Upper = 2, Bilateral = 4"

// ---------------------------------------------------------------------
