%feature("docstring") PERSALYS::MonteCarloAnalysis
"Perform a Monte Carlo central tendency analysis.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the Monte Carlo analysis:

>>> analysis = persalys.MonteCarloAnalysis('anAnalysis', myPhysicalModel)
>>> analysis.setMaximumCalls(500)
>>> analysis.setBlockSize(500)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> mean = result.getMean()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MonteCarloAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.DataAnalysisResult`
    Result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MonteCarloAnalysis::setLevelConfidenceInterval
"Confidence interval level accessor.

Parameters
----------
level : float
    Confidence interval level. It is set by default to 0.95."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MonteCarloAnalysis::getLevelConfidenceInterval
"Confidence interval level accessor.

Returns
-------
level : float
    Confidence interval level. It is set by default to 0.95."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MonteCarloAnalysis::setIsConfidenceIntervalRequired
"Confidence interval flag accessor.

Parameters
----------
flag : bool
    Confidence interval flag"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MonteCarloAnalysis::isConfidenceIntervalRequired
"Confidence interval flag accessor.

Returns
-------
flag : bool
    Confidence interval flag"
