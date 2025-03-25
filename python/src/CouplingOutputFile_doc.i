%feature("docstring") PERSALYS::CouplingOutputFile
"Output file metadata for coupling.

Parameters
----------
path : str
    Path to the output file.
    It must be must be relative, and is joined to the temporary working directory.

See Also
--------
CouplingStep, CouplingInputFile, CouplingResourceFile"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::setPath
"Path accessor.

Parameters
----------
path : str
    Path to the output file, must be relative."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::getPath
"Path accessor.

Returns
-------
path : str
    Path"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::setVariables
"Variables accessor.

Parameters
----------
names : sequence of str
    Variable names
tokens : sequence of str
    Tokens used for reading in the output file for each variable.
skip_lines : sequence of int
    Number of lines skipped for each variable.
skip_columns : sequence of int
    Number of columnes skipped for each variable."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::getTokens
"Tokens accessor.

Returns
-------
tokens : :py:class:`openturns.Description`
    Tokens for each variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::getVariableNames
"Variable names accessor.

Returns
-------
variable_names : :py:class:`openturns.Description`
    Output variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::getSkipLines
"Skip lines accessor.

Returns
-------
skip_lines : :py:class:`openturns.Point`
    Number of lines skipped for each variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingOutputFile::getSkipColumns
"Skip columns accessor.

Returns
-------
skip_columns : :py:class:`openturns.Point`
    Number of columns skipped for each variable"
