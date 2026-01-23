%feature("docstring") PERSALYS::DataModel
"Create a data model from an imported dataset.

The interface allows the user to load data samples and analyse them. They are considered as data models.

    DataModel(*name, fileName, inputColumns, outputColumns, inputNames, outputNames*)

    DataModel(*name, inSample, outSample*)

    DataModel(*name, physicalModel, importedDataset, inputNames, outputNames*)

Parameters
----------
name : str
    Name
fileName : str
    Name of a data file (.txt ot .csv) to load
inputColumns : sequence of int
    Indices of columns of the input variables in file to consider
outputColumns : sequence of int (optional)
    Indices of columns of the output variables in file to consider (optional)
inputNames : sequence of str 
    Names of the input variables (optional)
outputNames : sequence of str
    Names of the output variables (optional)
physicalModel : :py:class:`persalys.PhysicalModel`
    Physical model associated to the data model
importedDataset : :py:class:`persalys.ImportedDataset`
    Imported dataset associated to the data model (optional)
inSample : :py:class:`openturns.Sample`
    Input sample (its description must be a list of input variable names)
outSample : :py:class:`openturns.Sample`
    Output sample (its description must be a list of output variable names)

Examples
--------
>>> import openturns as ot
>>> import persalys
>>> ot.RandomGenerator.SetSeed(0)
>>> fileName = 'sample.csv'
>>> sample = ot.Normal(3).getSample(30)
>>> sample.exportToCSVFile(fileName)
>>> model = persalys.DataModel('dataModel', fileName, [0, 2], [1], ['var1', 'var2'], ['var3'])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getInputNames
"Names of the input variables accessor.

Returns
-------
names : :py:class:`openturns.Description`
   Names of the input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getOutputNames
"Names of the output variables accessor.

Returns
-------
names : :py:class:`openturns.Description`
   Names of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::setColumns
"Columns and names of variables accessor.

Parameters
----------
inputColumns : sequence of int
    Columns of input variables
inNames : sequence of str
   Names of input variables
outputColumns : sequence of int
    Columns of output variables
outNames : sequence of str
   Names of output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getSampleFromFile
" Returns the sample contained inside the imported dataset.

Returns
-------
sample : :py:class:`openturns.Sample`
   Sample contained inside the imported dataset"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getInputColumns
"Indices of the input variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Indices of the input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getOutputColumns
"Indices of the output variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Indices of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::getFileName
"File name of the imported dataset accessor.

Returns
-------
fileName : str
   File name of the imported dataset"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataModel::setFileName
"Sets the file name of the imported dataset.

Parameters
----------
fileName : str
    File name of the imported dataset"