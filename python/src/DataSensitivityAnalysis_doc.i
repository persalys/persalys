%feature("docstring") PERSALYS::DataSensitivityAnalysis
"Perform a sensitivity analysis on a design of experiments.
see :py:class:`openturns.experimental.RankSobolSensitivityAlgorithm` for more details.

Parameters
----------
name : str
    Name of the analysis.
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments to analyze.
type : byte, optional
    Type of sensitivity analysis to perform, encoded in a byte with : 
    - persalys.DataSensitivityAnalysis.RankSobol for Rank Sobol indices (bit 0, 0b00000001)
    - persalys.DataSensitivityAnalysis.SRC for SRC indices (bit 1, 0b00000010)
    - persalys.DataSensitivityAnalysis.GlobalHSIC for global HSIC indices (bit 2, 0b00000100)
    The default value is 0b00000011 (Rank Sobol and SRC indices are computed by default).
covarianceModels : collection of :class:`~openturns.CovarianceModel`, optional
    Collection of covariance models to use for HSIC indices computation. 
    The size of the collection must be equal to the total number of input and output variables in the design of experiment. 
estimateCovModelParameters : bool, optional
    Whether to estimate the covariance model parameters from the samples. 
    If false, the covariance models are used with their current parameters.
    If true, the scale parameter of each covariance model is set to the standard deviation of the corresponding variable in the sample.
    The default value is true.

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

>>> importedDataset = persalys.ImportedDataset('data.csv', [0, 1, 2], [3])
>>> model = persalys.DataModel('myDataModel', importedDataset)

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
