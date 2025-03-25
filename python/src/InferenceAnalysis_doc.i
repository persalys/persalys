%feature("docstring") PERSALYS::InferenceAnalysis
"Perform a Kolmogorov goodness-of-fit test for 1-D continuous distributions.

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

Create the Inference Analysis:

>>> analysis = persalys.InferenceAnalysis('analysis', model)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> resultX0 = result.getFittingTestResultForVariable('X0')"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceAnalysis::getLevel
R"RAW(Level accessor.

Returns
-------
level : float, :math:`0 < {\rm level} < 1`, optional
    The risk of committing a Type I error, that is an incorrect rejection of a true
    null hypothesis)RAW"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceAnalysis::setLevel
R"RAW(Level accessor.

Parameters
----------
level : float, :math:`0 < {\rm level} < 1`, optional
    The risk of committing a Type I error, that is an incorrect rejection of a true
    null hypothesis)RAW"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceAnalysis::getDistributionsFactories
"Get the sequence of distributions to test for a variable.

Parameters
----------
variable : str
    Name of the variable

Returns
-------
factories : sequence of :py:class:`openturns.DistributionFactory`
    Distributions to test"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceAnalysis::setDistributionsFactories
"Set the sequence of distributions to test for a variable.

Parameters
----------
variable : str
    Name of the variable
factories : sequence of :py:class:`openturns.DistributionFactory`
    Distributions to test"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::InferenceAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.InferenceResult`
    Result."
