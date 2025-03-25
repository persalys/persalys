%feature("docstring") PERSALYS::CouplingInputFile
"Input file metadata for coupling.

Parameters
----------
path : str
    Path to the template file.

Notes
-----
By default the configured path is the basename of the template path,
minus the `.in` extension.

See Also
--------
CouplingStep, CouplingResourceFile, CouplingOutputFile"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::setPath
"Template file path accessor.

Parameters
----------
path : str
    Path to the template file."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::getPath
"Template file path accessor.

Returns
-------
path : str
    Path to the template file."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::setConfiguredPath
"Configured path accessor.

Parameters
----------
configured_path : str
    Configured file path, must be relative.
    It is interpreted as the path to the file with the actual input values
    relatively to the temporary working directory."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::getConfiguredPath
"Template path accessor.

Returns
-------
configured_path : str
    Configured file path."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::setVariables
"Variables accessor.

Parameters
----------
names : sequence of str
    Variable names
tokens : sequence of str
    Tokens used for replacement in the template file.
formats : sequence of str
    Variable numerical formats"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::getTokens
"Tokens accessor.

Returns
-------
tokens : :py:class:`openturns.Description`
    Tokens for each variable"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::getVariableNames
"Variable names accessor.

Returns
-------
variable_names : :py:class:`openturns.Description`
    Input variable names"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingInputFile::getFormats
"Variables format accessor.

Returns
-------
formats : :py:class:`openturns.Description`
    Input variables format"
