%feature("docstring") PERSALYS::GridMeshModel
"Create a model to build regular meshes.

Available constructors:
    GridMeshModel(*parameters, bounds, nbNodes*)

Parameters
----------
parameters : sequence of :class:`persalys.Variable`
    Sequence of index parameters (optional). By default
    the index parameter is named **t**
bounds : :py:class:`openturns.Interval`
    Mesh bounds along each direction
nbNodes : sequence of int
    Number of nodes along each direction

Notes
-----
The sequences must have a dimension equal to the number of index parameters.

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the mesh model:

>>> meshModel = persalys.GridMeshModel([persalys.Variable('x', 'aDescription')], ot.Interval(0., 12.), [10])

Create the model:

>>> z0 = persalys.Input('z0', 100)
>>> v0 = persalys.Input('v0', 55)
>>> m = persalys.Input('m', 80)
>>> c = persalys.Input('c', 15)
>>> z = persalys.Output('z')
>>> formula = ['max(0, z0 + (m * 9.81 / c) * x + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-x * c / m)))']
>>> model = persalys.SymbolicFieldModel('model', meshModel, [z0, v0, m, c], [z], formula)

>>> y = model.getPointToFieldFunction()([100, 55, 80, 15])"


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridMeshModel::setParameters
"Parameters accessor.

Parameters
----------
bounds : :py:class:`openturns.Interval`
    Mesh bounds along each direction
nbNodes : sequence of int
    Number of nodes along each direction"
