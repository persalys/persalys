%feature("docstring") PERSALYS::DataAnalysis
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

Create the Data analysis:

>>> analysis = persalys.DataAnalysis('analysis', model)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> mean = result.getMean()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysis::getLevelConfidenceInterval
"Confidence interval level accessor.

Returns
-------
value : float
    Confidence interval level."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysis::setLevelConfidenceInterval
"Confidence interval level accessor.

Parameters
----------
value : float
    Confidence interval level"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysis::isConfidenceIntervalRequired
"Whether a confidence interval is required.

Returns
-------
value : bool
    Whether a confidence interval is required."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysis::setIsConfidenceIntervalRequired
"Whether a confidence interval is required.

Parameters
----------
value : bool
    Whether a confidence interval is required"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.DataAnalysisResult`
    Result."
