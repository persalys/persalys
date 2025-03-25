%feature("docstring") PERSALYS::Study
"Create the class to store a study."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::Add
"Add a study in the tree view.

Parameters
----------
object : :class:`~persalys.Study`
    The study to add"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::GetFileNames
"Files name accessor.

Returns
-------
filesNames : :py:class:`openturns.Description`
    The file names of all the opened studies"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::Open
"Open a study.

Parameters
----------
fileName : str
    The file name

Returns
-------
study : :class:`~persalys.Study`
    The study saved in fileName"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::save
"Save it in an xml file.

Parameters
----------
fileName : str
    The file name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::Remove
"Remove a study from the tree view.

Parameters
----------
object : :class:`~persalys.Study`
    The study to remove"
 
// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::GetInstances
"Studies accessor.

Returns
-------
studies : sequence of :class:`~persalys.Study`
    Studies existing in the tree view"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::GetInstanceByName
"Study accessor.

Parameters
----------
name : str
    The study name

Returns
-------
study : :class:`~persalys.Study`
    Study"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::GetAvailableName
"Available name for a study accessor.

Returns
-------
name : str
    Available name for a study"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::HasInstanceNamed
"Whether the tree view contains a study named with the given name.

Parameters
----------
name : str
    A study name

Returns
-------
contains : bool
    Whether it contains a study named with the given name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::add
"Add an object to the study.

Parameters
----------
object : :class:`~persalys.PhysicalModel`, :class:`~persalys.LimitState`, :class:`~persalys.Analysis` ...
    The object to add"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::remove
"Remove an object from the study.

Parameters
----------
object : :class:`~persalys.PhysicalModel`, :class:`~persalys.LimitState`, :class:`~persalys.Analysis` ...
    The object to remove"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getPythonScript
"Python script accessor.

Returns
-------
script : str
    The Python script"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getPythonScript
"Python script accessor.

Returns
-------
script : str
    The Python script"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::setFileName
"File name accessor.

Parameters
----------
fileName : str
    The file name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getFileName
"File name accessor.

Returns
-------
fileName : str
    The file name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getPhysicalModels
"Physical models accessor.

Returns
-------
models : sequence of :class:`~persalys.PhysicalModel`
    Physical models"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getLimitStates
"Limit state accessor.

Returns
-------
states : sequence of :class:`~persalys.LimitState`
    Limit states"
 
// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAnalyses
"Analysis accessor.

Returns
-------
analyses : sequence of :class:`~persalys.Analysis`
    Analyses"
    
// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getDesignOfExperiments
"Designs of experiments accessor.

Returns
-------
designs : sequence of :class:`~persalys.DesignOfExperiment`
    Designs of experiments"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getDataModels
"Data model accessor.

Returns
-------
models : sequence of :class:`~persalys.DataModel`
    Data models"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getPhysicalModelByName
"Physical model accessor.

Parameters
----------
name : str
    The model name

Returns
-------
model : :class:`~persalys.PhysicalModel`
    Physical model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getDesignOfExperimentByName
"Design of experiments accessor.

Parameters
----------
name : str
    The design of experiments name

Returns
-------
design : :class:`~persalys.DesignOfExperiment`
    Design of experiments"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAnalysisByName
"Analysis accessor.

Parameters
----------
name : str
    The analysis name

Returns
-------
analysis : :class:`~persalys.Analysis`
    Analysis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getDataModelByName
"Data model accessor.

Parameters
----------
name : str
    The data model name

Returns
-------
model : :class:`~persalys.DataModel`
    Data model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAvailableAnalysisName
"Available name for an analysis accessor.

Returns
-------
name : str
    Available name for an analysis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAvailableDataModelName
"Available name for a data model accessor.

Returns
-------
name : str
    Available name for a data model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAvailableDesignOfExperimentName
"Available name for a design of experiments accessor.

Returns
-------
name : str
    Available name for a design of experiments"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAvailableLimitStateName
"Available name for a limit state accessor.

Returns
-------
name : str
    Available name for a limit state"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::getAvailablePhysicalModelName
"Available name for a physical model accessor.

Returns
-------
name : str
    Available name for a physical model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::hasAnalysisNamed
"Whether it contains an analysis named with the given name.

Parameters
----------
name : str
    An analysis name

Returns
-------
contains : bool
    Whether it contains an analysis named with the given name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::hasDataModelNamed
"Whether it contains a data model named with the given name.

Parameters
----------
name : str
    A data model name

Returns
-------
contains : bool
    Whether it contains a data model named with the given name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::hasPhysicalModelNamed
"Whether it contains a physical model named with the given name.

Parameters
----------
name : str
    A physical model name

Returns
-------
contains : bool
    Whether it contains a physical model named with the given name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::hasDesignOfExperimentNamed
"Whether it contains a design of experiments named with the given name.

Parameters
----------
name : str
    A design of experiments name

Returns
-------
contains : bool
    Whether it contains a design of experiments named with the given name"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::Study::hasLimitStateNamed
"Whether it contains a limit state named with the given name.

Parameters
----------
name : str
    A limit state name

Returns
-------
contains : bool
    Whether it contains a limit state named with the given name"



