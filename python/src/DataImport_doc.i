%feature("docstring") PERSALYS::DataImport
""

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::getInputColumns
"Columns of the input variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Columns of the input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::getOutputColumns
"Columns of the output variables accessor.

Returns
-------
columns : :py:class:`openturns.Indices`
   Columns of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::setColumns
"Columns of the variables accessor.

Parameters
----------
inputColumns : sequence of int
    Columns of the input variables
outputColumns : sequence of int, optional
    Columns of the output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::getFileName
"File name accessor.

Returns
-------
fileName : str
   Name of the file containing data"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::setFileName
"File name accessor.

Parameters
----------
fileName : str
   Name of the file containing data"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::DataImport::getSampleFromFile
"Sample from the file accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
   Sample from the file"
