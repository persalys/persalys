%feature("docstring") PERSALYS::PythonFieldModel
R"RAW(Create a physical model defined by Python code and a mesh.

Parameters
----------
name : str
    Name
inputs : sequence of :class:`~persalys.Input`
    Input variables (optional)
outputs : sequence of :class:`~persalys.Output`
    Output variables (optional)
code : str
    Code string (optional)

Notes
-----
The code string must contain a function named '_exec' that takes float
arguments and return sequences of floats. The mesh is set as a global
variable of type :py:class:`openturns.Mesh` named 'mesh'.
The code can use this variable and retrieve the mesh vertices as follows:
:math:`aVar = getMesh().getVertices()`

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
>>> code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    return z'
>>> model = persalys.PythonFieldModel('aModel', meshModel, [z0, v0, m, c], [z], code)

>>> y = model.getPointToFieldFunction()([100, 55, 80, 15])
)RAW"
