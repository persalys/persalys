%feature("docstring") PERSALYS::Observations
"Create observations for variables of a model.

Available constructors:
    Observations(*name, physicalModel, fileName, inputColumns, outputColumns, inputNames, outputNames*)

    Observations(*name, inputSample, outputSample*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
fileName : str
    Name of a data file (.txt ot .csv) to load
inputColumns : sequence of int
    Indices of columns of the input variables in file to consider
outputColumns : sequence of int
    Indices of columns of the output variables in file to consider (optional)
inputNames : sequence of str
    Names of the input variables (optional)
outputNames : sequence of str
    Names of the output variables (optional)
inputSample : :py:class:`openturns.Sample`
    Input sample (its description must be a list of input variable names)
outputSample : :py:class:`openturns.Sample`
    Output sample (its description must be a list of output variable names)

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> ot.RandomGenerator.SetSeed(0)

Create the model:

>>> X0 = persalys.Input('X0')
>>> X1 = persalys.Input('X1')
>>> X2 = persalys.Input('X2')
>>> X3 = persalys.Input('X3')
>>> Y0 = persalys.Output('Y0')
>>> Y1 = persalys.Output('Y1')
>>> model = persalys.SymbolicPhysicalModel('aModelPhys', [X0, X1, X2, X3], [Y0, Y1], ['sin(X0)+8*X1', 'X2 + X3'])

Create the observations:

>>> filename = 'data.csv'
>>> ot.Normal(8).getSample(10).exportToCSVFile(filename)
>>> aObs = persalys.Observations('anObs', model, filename, [2, 7], [3], ['X0', 'X2'], ['Y1'])"

