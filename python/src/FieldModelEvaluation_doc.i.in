%feature("docstring") PERSALYS::FieldModelEvaluation
"Generate the evaluation of a model with mesh.

Parameters
----------
name : str
    Name
model : :class:`~persalys.PhysicalModel`
    Model containing a mesh
values : sequence of float
    Input values (optional)

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> meshModel = persalys.GridMeshModel(ot.Interval(0., 12.), [5])

>>> z0 = persalys.Input('z0', 100, '')
>>> v0 = persalys.Input('v0', 55, '')
>>> m = persalys.Input('m', 80, '')
>>> c = persalys.Input('c', 15, '')
>>> z = persalys.Output('z', '')
>>> formula = ['max(0, z0 + (m * 9.81 / c) * t + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-t * c / m)))']

>>> model = persalys.SymbolicFieldModel('aModel', meshModel, [z0, v0, m, c], [z], formula)

Create the analysis:

>>> analysis = persalys.FieldModelEvaluation('analysis', model)
>>> analysis.run()

Get the result:

>>> result = analysis.getResult()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::FieldMonteCarloResult::getProcessSample
"Output values accessor.

Returns
-------
sample : :py:class:`openturns.ProcessSample`
    Output values"

// ---------------------------------------------------------------------
