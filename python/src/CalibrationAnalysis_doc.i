%feature("docstring") PERSALYS::CalibrationAnalysis
"Run a calibration analysis.

Parameters
----------
name : str
    Name
observations : :class:`~persalys.Observations`
    Observations of at least one input and one output

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the Chaboche model:

>>> R = persalys.Input('R', 700e6, ot.LogNormalMuSigma(750e6, 11e6).getDistribution(), 'Parameter R')
>>> C = persalys.Input('C', 2500e6, ot.Normal(2750e6, 250e6), 'Parameter C')
>>> gamma = persalys.Input('gam', 8., ot.Normal(10, 2), 'Parameter gamma')
>>> dist_strain = ot.Uniform(0, 0.07)
>>> strain = persalys.Input('strain', 0., dist_strain, 'Strain')
>>> sigma = persalys.Output('sigma', 'Stress (Pa)')

>>> model = persalys.SymbolicPhysicalModel('model1', [R, C, gamma, strain], [sigma], ['R + C / gam * (1 - exp(-gam * strain))'])

Generate observations:

>>> nbObs = 100
>>> strainObs = dist_strain.getSample(nbObs)
>>> strainObs.setDescription(['strain'])
>>> stressSampleNoise = ot.Normal(0., 40.e6).getSample(nbObs)
>>> stressSample = ot.ParametricFunction(model.getFunction(), [0, 1, 2], [750e6, 2750e6, 10.])(strainObs)
>>> stressObs = stressSample + stressSampleNoise

>>> observations = persalys.Observations('obs1', model, strainObs, stressObs)

Process a calibration analysis:

>>> analysis = persalys.CalibrationAnalysis('myAnalysis', observations)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> thetaStar = result.getCalibrationResult().getParameterMAP()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::GetMethodNames
"Get the list of available method names.

Returns
-------
names : :class:`~openturns.Description`
    List of available calibration method names."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getMethodName
"Accessor to the method name

Returns
-------
method : str
    Method name. Default is 'LeastSquaresLinear'.
    Use :func:`GetMethodNames()` to list available names."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setMethodName
"Accessor to the method name

Parameters
----------
method : str
    Method name. Default is 'LeastSquaresLinear'.
    Use :func:`GetMethodNames()` to list available names."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getObservations
"Accessor to the data to be fitted.

Returns
-------
data : :class:`~openturns.Sample`
    The data to be fitted."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setObservations
"Accessor to the data to be fitted.

Returns
-------
data : :class:`~openturns.Sample`
    The data to be fitted."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getPriorDistribution
"Accessor to the prior distribution.

Returns
-------
distribution : :py:class:`openturns.Distribution`
    Prior distribution"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getErrorCovariance
"Accessor to the output observations error covariance.

Returns
-------
matrix : :py:class:`openturns.CovarianceMatrix`
    The covariance matrix of the gaussian distribution of the output observations error.
    Used by the gaussian algorithms."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setErrorCovariance
"Accessor to the output observations error covariance.

Parameters
----------
matrix : :py:class:`openturns.CovarianceMatrix`
    The covariance matrix of the gaussian distribution of the output observations error.
    Used by the gaussian algorithms."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getOptimizationAlgorithm
"Accessor to the optimization algorithm used by non linear algorithms.

Returns
-------
algorithm : :py:class:`openturns.OptimizationAlgorithm`
    Optimization algorithm used by non linear algorithms.
    The optimization problem and the starting point are not taken into account."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setOptimizationAlgorithm
"Accessor to the optimization algorithm used by non linear algorithms.

Parameters
----------
algorithm : :py:class:`openturns.OptimizationAlgorithm`
    Optimization algorithm used by non linear algorithms.
    The optimization problem and the starting point are not taken into account."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setConfidenceIntervalLength
"Confidence interval length accessor.

Parameters
----------
length : float
    Length of the confidence interval of the posterior distribution.
    It is set by default to 0.95."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getConfidenceIntervalLength
"Confidence interval length accessor.

Returns
-------
length : float
    Length of the confidence interval of the posterior distribution.
    It is set by default to 0.95."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setBootStrapSize
"Accessor to the bootstrap size used to sample the posterior distribution.

Parameters
----------
size : int
    Bootstrap size used to sample the posterior distribution by the non linear algorithms.
    A value of 0 means that no bootstrap has been done but a linear approximation
    has been used to get the posterior distribution, using the
    GaussianLinearCalibration or LinearLeastSquaresCalibration algorithm
    at the maximum a posteriori estimate. The value 1 is not allowed at this time."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getBootStrapSize
"Accessor to the bootstrap size used to sample the posterior distribution.

Returns
-------
size : int
    Bootstrap size used to sample the posterior distribution by the non linear algorithms.
    A value of 0 means that no bootstrap has been done but a linear approximation
    has been used to get the posterior distribution, using the
    GaussianLinearCalibration or LinearLeastSquaresCalibration algorithm
    at the maximum a posteriori estimate. The value 1 is not allowed at this time."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getFixedInputs
"Fixed inputs accessor.

Returns
-------
inputs : :py:class:`openturns.PointWithDescription`
    Fixed input names and values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getCalibratedInputs
"Inputs to calibrate accessor.

Returns
-------
inputs : :py:class:`openturns.Description`
    Names of the input variables to calibrate"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::setCalibratedInputs
"Accessor to the inputs to calibrate and the fixed inputs.

Parameters
----------
calibratedInput : sequence of str
    Names of the input variables to calibrate
priorDistribution : :py:class:`openturns.Distribution`
    Prior distribution. Its dimension must equal to the number of
    variables to calibrate
fixedInputs : sequence of str, optional
    Names of the input variables to fix
values : sequence of float, optional
    Values of the fixed inputs"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CalibrationAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.CalibrationAnalysisResult`
    Result"
