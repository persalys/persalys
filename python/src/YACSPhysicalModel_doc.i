%feature("docstring") PERSALYS::YACSPhysicalModel
"Python physical model inteded to run on a cluster. Uses YACS scheme."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::getCode
"Script content of the model.

Returns
-------
result : str
    function script as a string"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::setCode
"Modify the script content of the model.

Parameters
----------
script : str
    function script as a string"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::jobParameters
"Launching parameters accessor.

Returns
-------
result : :py:class:'pydefx.parameters.Parameters'
    job parameters structure"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::setJobModel
"Set the job model which drives the execution of the job.

Parameters
----------
model : :py:class:`pydefx.pystudy.PyStudy`
    job model, e.g. `pydefx.PyStudy`, `pydefx.LocalStudy`"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::setUseYACS
"Launching parameters accessor.

Parameters
----------
useYACS : bool
    Whether to use the YACS backend instead of the Python backend"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSPhysicalModel::getUseYACS
"Launching parameters accessor.

Returns
-------
useYACS : bool
    Whether to use the YACS backend instead of the Python backend"
