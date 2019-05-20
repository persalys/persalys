%feature("docstring") PERSALYS::SRCAnalysis
"Run a SRC sensitivity analysis.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
simulationsNumber : int
    Simulations number (optional). By default it is 10000.

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the SRC analysis:

>>> analysis = persalys.SRCAnalysis('anAnalysis', myPhysicalModel, 1000)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()
>>> indices = result.getIndices()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`~persalys.SRCResult`
    Result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCAnalysis::getSimulationsNumber
"Simulations number accessor.

Returns
-------
number : int
    Number of simulations"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SRCAnalysis::setSimulationsNumber
"Simulations number accessor.

Parameters
----------
number : int
    Number of simulations. By default it is 10000."
