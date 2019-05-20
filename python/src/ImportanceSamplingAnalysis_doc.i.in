%feature("docstring") PERSALYS::ImportanceSamplingAnalysis
"Run importance sampling for reliability analysis.

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

>>> analysis = persalys.ImportanceSamplingAnalysis('myAnalysis', limitState)
>>> analysis.setStandardSpaceDesignPoint([-1.59388,0.998942])
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> pf = result.getSimulationResult().getProbabilityEstimate()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportanceSamplingAnalysis::getStandardSpaceDesignPoint
"Accessor to standard space design point.

Returns
-------
point : :py:class:`openturns.Point`
    Standard space design point. It is the mean of the Normal distribution used to sample."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportanceSamplingAnalysis::setStandardSpaceDesignPoint
"Accessor to standard space design point.

Parameters
----------
point : sequence of float
    Standard space design point. It is the mean of the Normal distribution used to sample"
