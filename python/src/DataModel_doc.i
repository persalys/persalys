%feature("docstring") PERSALYS::DataModel
"Create a data model from an imported dataset.

The interface allows the user to load data samples and analyse them. They are considered as data models.

    DataModel(*name, fileName, inputColumns, outputColumns, inputNames, outputNames*)

    DataModel(*name, inputSample, outputSample*)

Parameters
----------
name : str
    Name
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
    Input sample
outputSample : :py:class:`openturns.Sample`
    Output sample (optional)

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

%feature("docstring") PERSALYS::DataImport::setColumns
"Columns and names of variables accessor.

Parameters
----------
inputColumns : sequence of int
    Columns of input variables
inNames : sequence of str
   Names of input variables
outputColumns : sequence of int, optional
    Columns of output variables
outNames : sequence of str
   Names of output variables"

