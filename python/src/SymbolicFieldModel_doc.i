%feature("docstring") PERSALYS::SymbolicFieldModel
"Create a physical model defined by analytical formulas and a mesh.

Parameters
----------
name : str
    Name
mesh : :class:`~persalys.MeshModel`
    Mesh
inputs : sequence of :class:`~persalys.Input`
    Input variables (optional)
outputs : sequence of :class:`~persalys.Output`
    Output variables (optional)
formulas : sequence of str
    List of analytical formulas between the inputs and the outputs (optional).
    The function is defined by outputs = formulas(inputs).


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

