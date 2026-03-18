%feature("docstring") PERSALYS::DataSensitivityAnalysis
"Perform a sensitivity analysis on a design of experiments.
see :py:class:`openturns.experimental.RankSobolSensitivityAlgorithm` for more details.

Parameters
----------
name : str
    Name of the analysis.
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments to analyze.
type : int, optional
    Type of sensitivity analysis to perform, encoded in a byte with : 
    - persalys.DataSensitivityAnalysisResult.RankSobol for Rank Sobol indices
    - persalys.DataSensitivityAnalysisResult.SRC for SRC indices
    - persalys.DataSensitivityAnalysisResult.GlobalHSIC for global HSIC indices
    - persalys.DataSensitivityAnalysisResult.TargetHSIC for target HSIC indices
    - persalys.DataSensitivityAnalysisResult.ConditionalHSIC for conditional HSIC indices
    The default value is RankSobol | SRC (Rank Sobol and SRC indices are computed by default).
covarianceModels : collection of :class:`~openturns.CovarianceModel`, optional
    Collection of covariance models to use for HSIC indices computation. 
    The size of the collection must be equal to the total number of input and output variables in the design of experiment.
interestVariables : :class:`~openturns.Description`, optional
    Description of the variables of interest for the sensitivity analysis. 
    The variables of interest must be a subset of the output variables in the design of experiment. 
    If not provided, all output variables will be considered as variables of interest.
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

>>> analysis = persalys.DataSensitivityAnalysis('sensitivity', model, persalys.DataSensitivityAnalysisResult.RankSobol | persalys.DataSensitivityAnalysisResult.SRC)
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

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setType
"Set the type of sensitivity analysis to perform.

Parameters
----------
analysisType : int
    Type of sensitivity analysis to perform, encoded in a byte with :
- persalys.DataSensitivityAnalysis.RankSobol for Rank Sobol indices
- persalys.DataSensitivityAnalysis.SRC for SRC indices
- persalys.DataSensitivityAnalysis.GlobalHSIC for global HSIC indices
- persalys.DataSensitivityAnalysis.TargetHSIC for target HSIC indices
- persalys.DataSensitivityAnalysis.ConditionalHSIC for conditional HSIC indices"

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setCovarianceModels
"Set the covariance models used for the computation of HSIC indices.

Parameters
----------
covarianceModels : collection of :class:`~openturns.CovarianceModel`
    Collection of covariance models to use for the computation of HSIC indices. 
    The size of the collection must be equal to the total number of input and output variables in the design of experiment.
hsicType : int
    Type of HSIC indices (Global, Target or Conditional)."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setFilterAlphas
"Set the alpha parameters for the target HSIC filter functions.

The filter functions are constructed at launch time as :math:`x \\mapsto \\exp(-x / (\\alpha_i \\cdot \\sigma_i))`
where :math:`\\alpha_i` is the alpha parameter and :math:`\\sigma_i` is the standard deviation of the
i-th output variable. Setting alphas clears any previously set filter functions.

Parameters
----------
filterAlphas : :class:`~openturns.Point`
    Alpha parameters for each output variable of interest.
    The size must be equal to the number of output variables of interest."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setWeightAlphas
"Set the alpha parameters for the conditional HSIC weight functions.

The weight functions are constructed at launch time as :math:`x \\mapsto \\exp(-x / (\\alpha_i \\cdot \\sigma_i))`
where :math:`\\alpha_i` is the alpha parameter and :math:`\\sigma_i` is the standard deviation of the
i-th output variable. Setting alphas clears any previously set weight functions.

Parameters
----------
weightAlphas : :class:`~openturns.Point`
    Alpha parameters for each output variable of interest.
    The size must be equal to the number of output variables of interest."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getFilterAlphas
"Get the alpha parameters for the target HSIC filter functions.

Returns
-------
filterAlphas : :class:`~openturns.Point`
    Alpha parameters for each output variable of interest. Empty if filter functions were set directly."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getWeightAlphas
"Get the alpha parameters for the conditional HSIC weight functions.

Returns
-------
weightAlphas : :class:`~openturns.Point`
    Alpha parameters for each output variable of interest. Empty if weight functions were set directly."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setFilterFunctions
"Set the filter functions for the target HSIC indices computation.

Setting filter functions directly clears any previously set filter alphas.
For most use cases, prefer :meth:`setFilterAlphas` which allows the analysis
to construct the functions automatically.

Parameters
----------
filterFunctions : collection of :class:`~openturns.Function`
    Collection of filter functions to use for the computation of target HSIC indices.
    The size of the collection must be equal to the number of output variables of interest."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getFilterFunctions
"Get the filter functions for the target HSIC indices computation.

Returns
-------
filterFunctions : collection of :class:`~openturns.Function`
    Collection of filter functions. May be empty if filter alphas were set
    and the analysis has not been launched yet."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setWeightFunctions
"Set the weight functions for the conditional HSIC indices computation.

Setting weight functions directly clears any previously set weight alphas.
For most use cases, prefer :meth:`setWeightAlphas` which allows the analysis
to construct the functions automatically.

Parameters
----------
weightFunctions : collection of :class:`~openturns.Function`
    Collection of weight functions to use for the computation of conditional HSIC indices.
    The size of the collection must be equal to the number of output variables of interest."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getWeightFunctions
"Get the weight functions for the conditional HSIC indices computation.

Returns
-------
weightFunctions : collection of :class:`~openturns.Function`
    Collection of weight functions. May be empty if weight alphas were set
    and the analysis has not been launched yet."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::setHSICParameters
"Set the parameters for the HSIC indices computation.

Parameters
----------
computePermutationPValues : bool
    Whether to compute permutation p-values.
computeAsymptoticPValues : bool
    Whether to compute asymptotic p-values.
useUStatistic : bool
    Whether to use the U-statistic for the computation. If false, the V-statistic will be used.
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).
    For Conditional, asymptotic p-values and U-statistic are not supported and will be ignored."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getResult
"Get the result of the sensitivity analysis.

Returns
-------
result : :class:`~persalys.DataSensitivityAnalysisResult`
    Result of the sensitivity analysis."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::computeRankSobol
"Whether to compute Rank Sobol indices.

Returns
-------
compute : bool
    True if Rank Sobol indices will be computed, false otherwise."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::computeSRC
"Whether to compute SRC indices.

Returns
-------
compute : bool
    True if SRC indices will be computed, false otherwise."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::computeHSIC
"Whether to compute HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
compute : bool
    True if HSIC indices of the given type will be computed, false otherwise."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::computeAsymptoticPValues
"Whether to compute asymptotic p-values for HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
compute : bool
    True if asymptotic p-values will be computed, false otherwise."

// --------------------------------------------------------------------------- 

%feature("docstring") PERSALYS::DataSensitivityAnalysis::computePermutationPValues
"Whether to compute permutation p-values for HSIC indices of the given type.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
compute : bool
    True if permutation p-values will be computed, false otherwise."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::useUStatistic
"Whether to use the U-statistic for the computation of HSIC indices.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
useUStatistic : bool
    True if the U-statistic will be used, false if the V-statistic will be used."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::getCovarianceModels
"Get the covariance models used for the computation of HSIC indices.

Parameters
----------
hsicType : int
    Type of HSIC indices (Global, Target or Conditional).

Returns
-------
covarianceModels : collection of :class:`~openturns.CovarianceModel`
    Collection of covariance models used for the computation of HSIC indices."

// ---------------------------------------------------------------------------

%feature("docstring") PERSALYS::DataSensitivityAnalysis::defaultHSICParametersChanged
"Whether the default parameters for HSIC indices computation have been changed.

Returns
-------
changed : bool
    True if the default parameters for HSIC indices computation have been changed, false otherwise."

