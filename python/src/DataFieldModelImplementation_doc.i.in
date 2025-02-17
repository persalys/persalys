%define PERSALYS_DataFieldModel_doc
"Defines field evaluation from external data and mesh.

Parameters
----------
name : str
    Name
meshModel : :class:`~persalys.MeshModel`
    Mesh model indexing data
processSample : :class:`~openturns.ProcessSample`
    Field data, (optional)

Examples
--------
>>> import persalys
>>> import openturns as ot
>>> meshModel = persalys.GridMeshModel([persalys.Variable('t', 'date')], ot.Interval([1], [12]), [12])
>>> model = persalys.DataFieldModel('myModel', meshModel)
>>> fileName = 'sample.csv'
>>> sample = ot.Normal(12).getSample(30)
>>> sample.exportToCSVFile(fileName)
>>> model.importProcessSample(fileName, persalys.Tools.Columns)"

%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation
PERSALYS_DataFieldModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_importProcessSample_doc
"Data import method.

Parameters
----------
fileName : str
    Name of the file containing field data.
order : int
    Wether data is mesh-indexed as lines or columns in the file (optional, default : persalys.Tools.Columns)"
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::importProcessSample
PERSALYS_DataFieldModel_importProcessSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_setMeshModel_doc
"Mesh model accessor.

Parameters
----------
meshModel : :class:`~persalys.MeshModel`
    Mesh model."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::setMeshModel
PERSALYS_DataFieldModel_setMeshModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_getMeshModel_doc
"Mesh model accessor.

Returns
-------
meshModel : :class:`~persalys.MeshModel`
    Mesh model."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::getMeshModel
PERSALYS_DataFieldModel_getMeshModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_getPythonScript_doc
"Python script accessor.

Returns
-------
script : str
    Python script to replay the model."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::getPythonScript
PERSALYS_DataFieldModel_getPythonScript_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_setSampleAsProcessSample_doc
"Process sample accessor.

Parameters
----------
sample : :class:`~openturns.Sample`
    Sample containing field data."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::setSampleAsProcessSample
PERSALYS_DataFieldModel_setSampleAsProcessSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_setProcessSample_doc
"Process sample accessor.

Parameters
----------
processSample : :class:`~openturns.ProcessSample`
    Process sample containing field data."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::setProcessSample
PERSALYS_DataFieldModel_setProcessSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_getProcessSample_doc
"Process sample accessor.

Returns
-------
processSample : :class:`~openturns.ProcessSample`
    Process sample containing field data."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::getProcessSample
PERSALYS_DataFieldModel_getProcessSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_getProcessSampleAsSample_doc
"Process sample accessor.

Returns
-------
sample : :class:`~openturns.Sample`
    Sample (dimension n-vertices, size n-trajectories) containing field data."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::getProcessSampleAsSample
PERSALYS_DataFieldModel_getProcessSampleAsSample_doc

// ---------------------------------------------------------------------

%define PERSALYS_DataFieldModel_isValid_doc
"Python script accessor.

Returns
-------
isValid : bool
    Wether the model data is valid or not. It must contain only valid numerical values and data must be correctly indexed on the mesh model."
%enddef
%feature("docstring") PERSALYS::DataFieldModelImplementation::isValid
PERSALYS_DataFieldModel_isValid_doc
