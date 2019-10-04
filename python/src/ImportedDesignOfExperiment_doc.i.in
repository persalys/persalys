%feature("docstring") PERSALYS::ImportedDesignOfExperiment
"Create a design of experiments, using an imported design.

Available constructors:
    ImportedDesignOfExperiment(*name, physicalModel, fileName, columns*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
fileName : str
    Name of a data file to load
columns : sequence of int
    Indices of columns in files to consider

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> ot.RandomGenerator.SetSeed(0)

Create the model:

>>> X0 = persalys.Input('X0')
>>> X1 = persalys.Input('X1')
>>> Y0 = persalys.Output('Y0')
>>> model = persalys.SymbolicPhysicalModel('aModelPhys', [X0, X1], [Y0], ['sin(X0)+8*X1'])

Create the design of experiments:

>>> filename = 'normal.csv'
>>> ot.Normal(3).getSample(10).exportToCSVFile(filename)
>>> aDesign = persalys.ImportedDesignOfExperiment('aDesign', model, filename, [0, 2])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::setInputColumns
"Input columns accessor.

Parameters
----------
columns : sequence of int
   Columns of the input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::getResult
"Result of the analysis of the data accessor.

Returns
-------
result : :class:`~persalys.DataAnalysisResult`
   Result of the analysis of the data"
