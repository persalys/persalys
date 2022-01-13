%feature("docstring") PERSALYS::AnsysParser
"Class used to read Ansys project files."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::loadData
"Reads the project file and fills in variables information.

Parameters
----------
fileName : str
    Name of the project file"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setInputVariables
"Sets input variables.

Parameters
----------
inputs : sequence of :class:`~persalys.Input`
    Input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getInputVariables
"Returns input variables.

Returns
-------
inputs : sequence of :class:`~persalys.Input`
    Input variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setOutputVariables
"Sets output variables.

Parameters
----------
outputs : sequence of :class:`~persalys.Output`
    Output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getOutputVariables
"Returns output variables.

Returns
-------
outputs : sequence of :class:`~persalys.Output`
    Output variables"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::generateTemplate
"Reads the project file and fills in variables information.

Parameters
----------
templateFileName : str
    Name of the template to generate. By default, a template file named input.txt.in is created next to the project (.wbpj) file
outputFileName : str
    Name of the Ansys design point output file. By default: output.csv"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getFullAnsysVersion
"Returns ansys version as found in the project file

Returns
-------
fullAnsysVersion : str
    Full ansys version used to create the project (ex: '2020 R2')."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getAnsysVersion
"Returns formatted ansys version (vXXX) to help generate default ansys solver path.

Returns
-------
ansysVersion : str
    Formatted ansys version (ex: returns 'v202' for full version '2020 R2')."

// ---------------------------------------------------------------------

%ignore PERSALYS::AnsysParser::getProjectDirectory;
%ignore PERSALYS::AnsysParser::getSystems;
%ignore PERSALYS::AnsysParser::setSystems;

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getModelFileName
"Returns the project file name.

Returns
-------
modelFileName : str
    Project file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setModelFileName
"Sets the project file name.

Parameters
----------
modelFileName : str
    Project file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getOutputFileName
"Returns the design point file name.

Returns
-------
outputFileName : str
    Ansys design point file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setOutputFileName
"Sets the design point file name.

Parameters
----------
outputFileName : str
    Ansys design point file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getTemplateFileName
"Returns ansys journal template file name.

Returns
-------
templateFileName : str
    Ansys template replay file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setTemplateFileName
"Sets ansys journal template file name.

Parameters
----------
templateFileName : str
    Ansys template replay file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::getExecutableFileName
"Returns ansys solver file name.

Returns
-------
executableFileName : str
    Ansys solver file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::setExecutableFileName
"Sets ansys solver file name.

Parameters
----------
executableFileName : str
    Ansys solver file name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::AnsysParser::populateCouplingStep
"Sets ansys solver file name.

Parameters
----------
model : :class:`~persalys.CouplingPhysicalModel`
    Model that contains the step to be populated.
indStep : int
    Index of the step to be populated.
templateFileName : str
    Input template file path to be generated (Optional, defaults to input.txt.in in the directory containing the ansys project file)."
