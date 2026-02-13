%feature("docstring") PERSALYS::ImportedDataset
"Class to import file data.

    ImportedDataset()
    
    ImportedDataset(*fileName, inputColumns, outputColumns*)
   
Parameters
----------
fileName : str
    Path to the file to import data from
inputColumns : sequence of int
    Columns of the input variables
outputColumns : sequence of int, optional
    Columns of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::getFileName
"File name accessor.

Returns
-------
fileName : str
   Name of the file containing data"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::setFileName
"File name accessor.

Parameters
----------
fileName : str
   Name of the file containing data

Returns
-------
validArg : bool
   True if the columns could be kept, False if they have been reset to default values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::getInputColumns
"Columns of the input variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Columns of the input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::getOutputColumns
"Columns of the output variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Columns of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::getSampleFromFile
"Sample from the file accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
   Sample from the file"

// ---------------------------------------------------------------------
%feature("docstring") PERSALYS::ImportedDataset::setSampleFromFile
"Sample from the file accessor.

Parameters
----------
sample : :py:class:`openturns.Sample`
   Sample from the file"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::setColumns
"Columns of the variables accessor.

Parameters
----------
inputColumns : sequence of int
    Columns of the input variables
outputColumns : sequence of int, optional
    Columns of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ImportedDataset::setNames
"Changes the description of the sample

Parameters
----------
inputNames : :py:class:`openturns.Description`
    Names of the input variables
outputNames : :py:class:`openturns.Description`
    Names of the output variables"