%feature("docstring") PERSALYS::SORMAnalysis
"Run a reliability analysis using the SORM method.

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

Process a reliability analysis using the SORM method:

>>> analysis = persalys.SORMAnalysis('myAnalysis', limitState)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult().getSORMResult()
>>> pf = result.getEventProbabilityBreitung()
>>> designPoint = result.getStandardSpaceDesignPoint()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SORMAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.SORMAnalysisResult`
    Result"
