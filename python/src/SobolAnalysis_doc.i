%feature("docstring") PERSALYS::SobolAnalysis
"Run a Sobol sensitivity analysis.

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

Create the Sobol analysis:

>>> analysis = persalys.SobolAnalysis('anAnalysis', myPhysicalModel)
>>> analysis.setMaximumCalls(500)
>>> analysis.setReplicationSize(125)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> indices = result.getFirstOrderIndices()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.SobolResult`
    Result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolAnalysis::getConfidenceLevel
"Confidence level accessor.

Returns
-------
level : float
    Confidence level"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolAnalysis::setConfidenceLevel
"Confidence level accessor.

Parameters
----------
level : float in ]0,1[
    Confidence level"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolAnalysis::getReplicationSize
"Replication size accessor.

Returns
-------
size : int
    Replication size"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SobolAnalysis::setReplicationSize
"Replication size accessor.

Parameters
----------
size : int > 1
    Replication size"
