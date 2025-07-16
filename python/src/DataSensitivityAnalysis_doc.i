%feature("docstring") PERSALYS::DataSensitivityAnalysis
"Perform a sensitivity analysis on a design of experiments.
see :py:class:`openturns.experimental.RankSobolSensitivityAlgorithm` for more details.

Parameters
----------
name : str
    Name of the analysis.
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments to analyze.

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> from openturns.usecases import ishigami_function

Create the csv:

>>> im = ishigami_function.IshigamiModel()
>>> x = im.inputDistribution.getSample(100)
>>> y = im.model(x)
>>> data_sample = x
>>> data_sample.stack(y)
>>> data_sample.exportToCSVFile('data.csv')

Create the model:

>>> model = persalys.DataModel('myDataModel', 'data.csv', [0, 1, 2], [3])

Create and run the analysis: 

>>> analysis = persalys.DataSensitivityAnalysis('sensitivity', model)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> if result.isIndependent():
...    # The inputs need to be independant for the result to be valid
...    firstOrderIndices = result.getFirstOrderIndices()
...    firstOrderIndicesInterval = result.getFirstOrderIndicesInterval()
... else:
...    independenceWarning = result.getIndependenceWarningMessage()
"

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getResult
"Get the result of the sensitivity analysis.

Returns
-------
result : :class:`~persalys.DataSensitivityAnalysisResult`
    Result of the sensitivity analysis.
"
