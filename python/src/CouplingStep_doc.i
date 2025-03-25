%feature("docstring") PERSALYS::CouplingStep
"Coupling step.

Represents one step of the coupling with one command to execute and the
associated input or output files.

Parameters
----------
command : str
    Command to execute
    By default it is run by the system else see :meth:`setIsShell`.
input_files : sequence of :class:`~persalys.CouplingInputFile`
    Input file metadatas
resource_files : sequence of :class:`~persalys.CouplingResourceFile`
    Resource file metadatas
output_files : sequence of :class:`~persalys.CouplingOutputFile`
    Output file metadatas

See Also
--------
CouplingPhysicalModel, CouplingInputFile, CouplingResourceFile, CouplingOutputFile"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setCommand
"Path accessor.

Parameters
----------
path : str
    Path"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getCommand
"Path accessor.

Returns
-------
path : :py:class:`openturns.Description`
    Path"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setIsShell
"Shell command property accessor.

Parameters
----------
is_shell : bool
    Whether the command should be interpreted by the shell"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getIsShell
"Shell command property accessor.

Returns
-------
is_shell : bool
    Whether the command should be interpreted by the shell"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getInputFiles
"Template path accessor.

Returns
-------
input_files : sequence of :class:`~persalys.CouplingInputFile`
    Input file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setInputFiles
"Tokens accessor.

Parameters
----------
input_files : sequence of :class:`~persalys.CouplingInputFile`
    Input file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getResourceFiles
"Template path accessor.

Returns
-------
resource_files : sequence of :class:`~persalys.CouplingResourceFile`
    Resource file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setResourceFiles
"Tokens accessor.

Parameters
----------
resource_files : sequence of :class:`~persalys.CouplingResourceFile`
    Resource file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setOutputFiles
"Variable names accessor.

Parameters
----------
output_files : sequence of :class:`~persalys.CouplingOutputFile`
    Output file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getOutputFiles
"Variable names accessor.

Returns
-------
output_files : sequence of :class:`~persalys.CouplingOutputFile`
    Output file metadatas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::setEnvironment
"Environment dictionary accessor.

Parameters
----------
keys : :py:class:`openturns.Description`
    Dictionary keys matching environment variables names
values : :py:class:`openturns.Description`
    Dictionary values matching environment variables values"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getEnvironmentKeys
"Environment variables names accessor.

Returns
-------
keys : :py:class:`openturns.Description`
    Dictionary keys matching environment variables name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingStep::getEnvironmentValues
"Environment variables values accessor.

Returns
-------
values : :py:class:`openturns.Description`
    Dictionary valuess matching environment variables values"
