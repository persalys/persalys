%feature("docstring") PERSALYS::FORMImportanceSamplingAnalysis
"Run a FORM analysis followed by an importance sampling for reliability analysis.

Parameters
----------
name : str
    Name
limitState : :class:`~persalys.LimitState`
    Limit state

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the limit state:

>>> limitState = persalys.LimitState('ls1', myPhysicalModel, 'G', ot.Less(), 0.)

Process the reliability analysis:

>>> analysis = persalys.FORMImportanceSamplingAnalysis('myAnalysis', limitState)
>>> analysis.setMaximumCalls(1000)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> pf = result.getSimulationResult().getProbabilityEstimate()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ApproximationAnalysis::getFORMResult
"FORM result accessor.

Returns
-------
result : :py:class:`openturns.FORMResult`
    Result of the FORM analysis"
