%feature("docstring") PERSALYS::TaylorExpansionMomentsAnalysis
"Get the Taylor expansions for moments estimation.

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

Create the Taylor expansion analysis:

>>> analysis = persalys.TaylorExpansionMomentsAnalysis('anAnalysis', myPhysicalModel)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> mean = result.getMeanFirstOrder()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::TaylorExpansionMomentsAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.TaylorExpansionMomentsResult`
    Result"
