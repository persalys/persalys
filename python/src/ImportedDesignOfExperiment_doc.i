%feature("docstring") PERSALYS::ImportedDesignOfExperiment
"Create a design of experiments, using an imported design.

Available constructors:
    ImportedDesignOfExperiment(*name, physicalModel*)
    ImportedDesignOfExperiment(*name, physicalModel, fileName, inputColumns, outputColumns*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
fileName : str
    Name of a data file to load
inputColumns : sequence of int
    Columns of the input variables
outputColumns : sequence of int
    Columns of the output variables (optional)

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

%feature("docstring") PERSALYS::DataImport::getResult
"Result of the analysis of the data accessor.

Returns
-------
result : :class:`~persalys.DataAnalysisResult`
   Result of the analysis of the data"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::getType
"Get the type of the imported design of experiments.

Returns
-------
type : :class:`~persalys.ImportedDesignOfExperiment.Type`
   Type of the imported design of experiments"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::setType
"Set the type of the imported design of experiments.
By default, the type is Monte-Carlo.

Parameters
----------
type : int
   Type of the imported design of experiments"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::getType
"Get the type of the imported design of experiments.

Returns
-------
type : int
   Type of the imported design of experiments"
