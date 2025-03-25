%feature("docstring") PERSALYS::YACSCouplingPhysicalModel
"File-exchange based coupling model using YACS.

YACS engine variant of :class:`~persalys.CouplingPhysicalModel`.

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

Notes
-----
The YACS variant does not allow paths to the template file to be relative.

See Also
--------
CouplingPhysicalModel"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSCouplingPhysicalModel::getCode
"Script content of the model.

Returns
-------
result : str
    function script as a string"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSCouplingPhysicalModel::setCode
"Modify the script content of the model.

Parameters
----------
script : str
    function script as a string"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::YACSCouplingPhysicalModel::jobParameters
"Launching parameters accessor.

Returns
----------
result : :py:class:'pydefx.parameters.Parameters'
    job parameters structure"

// ---------------------------------------------------------------------
