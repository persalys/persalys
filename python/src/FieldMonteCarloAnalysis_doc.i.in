%feature("docstring") PERSALYS::FieldMonteCarloAnalysis
"Run a Monte Carlo analysis for models with mesh.

Parameters
----------
name : str
    Name
model : :class:`~persalys.PhysicalModel`
    Model containing a mesh

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> meshModel = persalys.GridMeshModel(ot.Interval(0., 12.), [5])

>>> z0 = persalys.Input('z0', 100, ot.Uniform(100, 150))
>>> v0 = persalys.Input('v0', 55, ot.Normal(55, 10))
>>> m = persalys.Input('m', 80, ot.Normal(80, 8))
>>> c = persalys.Input('c', 15, ot.Uniform(0, 30))
>>> z = persalys.Output('z', '')
>>> formula = ['max(0, z0 + (m * 9.81 / c) * t + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-t * c / m)))']

>>> model = persalys.SymbolicFieldModel('aModel', meshModel, [z0, v0, m, c], [z], formula)

Create the analysis:

>>> analysis = persalys.FieldMonteCarloAnalysis('analysis', model)
>>> analysis.setMaximumCalls(30)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()"

//----------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::getKarhunenLoeveAnalysis
"Karhunen-Loeve analysis accessor.

Returns
-------
analysis : :class:`persalys.FieldKarhunenLoeveAnalysis`
    Karhunen-Loeve analysis"

// --------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::getKarhunenLoeveThreshold
"Accessor to the limit ratio on eigenvalues.

Returns
-------
threshold : float, positive
    The threshold s defined in :py:meth:`openturns.KarhunenLoeveAlgorithm.setThreshold`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::setKarhunenLoeveThreshold
"Accessor to the limit ratio on eigenvalues.

Parameters
----------
threshold : float, positive
    The threshold s defined in :py:meth:`openturns.KarhunenLoeveAlgorithm.getThreshold`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::getQuantileLevel
"Probability level accessor.

Returns
-------
level : float
    Probability level"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::setQuantileLevel
"Probability level accessor.

Parameters
----------
level : float, :math:`0 < p < 1`
    Probability level"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloAnalysis::getResult
"Result accessor.

Returns
-------
result : :class:`persalys.FieldMonteCarloResult`
    Result"
