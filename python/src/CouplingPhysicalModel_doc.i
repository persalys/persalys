%feature("docstring") PERSALYS::CouplingPhysicalModel
"File-exchange based coupling model.

This model allows one to wrap computer codes that read input values
and write output values in text files.
It is defined from a sequence steps, each step being defined from:

- a list of input files to copy, or input files in which some variables are written
- a command to launch the code
- a list of output files and the associated output variables to read.

Each step is run sequentially in a temporary working directory for each input point.
The name of the temporary working directory is computed from a hash of the input point.

Parameters
----------
name : str
    Model name
inputs : sequence of :class:`~persalys.Input`
    Input variables (optional)
outputs : sequence of :class:`~persalys.Output`
    Output variables (optional)
steps : sequence of :class:`~persalys.CouplingStep`
    List of steps to execute (optional)

See Also
--------
CouplingStep, CouplingInputFile, CouplingResourceFile, CouplingOutputFile

Examples
--------
>>> import persalys
>>> import sys

input.txt is generated from the existing input_template.txt by replacing the @XN tokens:

>>> input_file = persalys.CouplingInputFile('/tmp/input_template.txt')
>>> input_file.setConfiguredPath('input.txt')
>>> input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])

external_program.py reads input.txt and writes output.txt:

>>> resource_file = persalys.CouplingResourceFile('external_program.py')

output variables Yj are read from output.txt and delimited by tokens, no token/line/column skipped:

>>> output_file = persalys.CouplingOutputFile('output.txt')
>>> output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
>>> step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt', [input_file], [resource_file], [output_file])
>>> model = persalys.CouplingPhysicalModel('A', [step])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::getSteps
"Coupling steps accessor.

Returns
-------
steps : sequence of :class:`~persalys.CouplingStep`
    List of steps to execute"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::setSteps
"Coupling steps accessor.

Parameters
----------
steps : sequence of :class:`~persalys.CouplingStep`
    List of steps to execute"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::getCleanupWorkDirectory
"Cleanup work directory accessor.

Returns
-------
cleanup_work_dir : bool
    Whether the work directory is discarded"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::setCleanupWorkDirectory
"Cleanup work directory accessor.

Parameters
----------
cleanup_work_dir : bool
    Whether the work directory is discarded"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::getCacheInputFile
"Cache input file accessor.

Returns
-------
input_file : str
    Cache input file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::getCacheOutputFile
"Cache output file accessor.

Returns
-------
output_file : str
    Cache output file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::CouplingPhysicalModel::setCacheFiles
"Cache files accessor.

Parameters
----------
input_file : str
    Cache input file name.
output_file : str
    Cache output file name."
