%feature("docstring") PERSALYS::ImportedMeshModel
"Create a model with imported meshes

Available constructors:
    ImportedMeshModel(*filename, parameters, columns, order*)

Parameters
----------
fileName : *str*
    Name of a data file to load
parameters : *sequence of :class:`persalys.Variable`, optional*
    Sequence of index parameters. By default
    the index parameter is named **t**
columns : *sequence of int, optional*
    Indices of columns in files to consider. The sequence dimension
    must have a dimension equal to the number of index parameters
order : *int, optional*
    Data order in the file. Possible values are:
    - persalys.Tools.Unknown (default) : the file contains 1D data and order will be inferred automatically
    - persalys.tools.Columns : each time step is stored in a different columns
    - persalys.tools.Rows : each time step is stored in a different row

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the mesh model:

>>> mesh = ot.IntervalMesher(ot.Indices(1, 20)).build(ot.Interval(0., 12.))
>>> mesh.getVertices().exportToCSVFile('mesh.csv')

>>> meshModel = persalys.ImportedMeshModel('mesh.csv', [persalys.Variable('x', 'aDescription')], [0])

Create the model:

>>> z0 = persalys.Input('z0', 100)
>>> v0 = persalys.Input('v0', 55)
>>> m = persalys.Input('m', 80)
>>> c = persalys.Input('c', 15)
>>> z = persalys.Output('z')
>>> formula = ['max(0, z0 + (m * 9.81 / c) * x + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-x * c / m)))']
>>> model = persalys.SymbolicFieldModel('model', meshModel , [z0, v0, m, c], [z], formula)

>>> y = model.getPointToFieldFunction()([100, 55, 80, 15])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedMeshModel::setParameterColumns
"Columns of the index parameters accessor.

Parameters
----------
columns : sequence of int
   Indices of columns in file of the index parameters"
