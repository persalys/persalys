%feature("docstring") PERSALYS::ImportedDesignOfExperiment
"Create a design of experiments, using an imported design.

Available constructors:
    ImportedDesignOfExperiment(*name, physicalModel*)
    ImportedDesignOfExperiment(*name, physicalModel, fileName, inputColumns, outputColumns, type*)

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
type : int
    Type of the imported design of experiments (optional, default is GENERIC)

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

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::setColumns
"Set the columns of the variables.

Parameters
----------
inputColumns : sequence of int
    Columns of the input variables
outputColumns : sequence of int (optional)
    Columns of the output variables"

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

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::getImportedDataset
"Get the imported dataset.

Returns
-------
importedDataset : :class:`~persalys.ImportedDataset`
   The imported dataset"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDesignOfExperiment::setFileName
"Set the name of the data file to load.

Parameters
----------
fileName : str
    Name of a data file to load"