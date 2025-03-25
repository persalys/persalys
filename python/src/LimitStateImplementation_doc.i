%define PERSALYS_LimitState_doc
"Create a limit state.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
outputName : str
    Interest variable name
operator : :py:class:`~openturns.ComparisonOperator`
    Comparison operator against threshold (optional)
threshold : float
    Threshold value (optional)

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> physicalModel = persalys.SymbolicPhysicalModel('model1', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the limit state:

>>> limitState = persalys.LimitState('ls1', physicalModel, 'G', ot.Less(), 0.)"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation
PERSALYS_LimitState_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOutputName_doc
"Accessor to the name of output variable of interest in limit state.

Parameters
----------
outputName : str
    Interest variable name"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::setOutputName
PERSALYS_LimitState_setOutputName_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getOutputName_doc
"Accessor to the name of output variable of interest in limit state.

Returns
-------
outputName : str
    Interest variable name"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::getOutputName
PERSALYS_LimitState_getOutputName_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setThreshold_doc
"Threshold value for limit state accessor.

Parameters
----------
threshold : float
    Threshold value"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::setThreshold
PERSALYS_LimitState_setThreshold_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getThreshold_doc
"Threshold value for limit state accessor.

Returns
-------
threshold : float
    Threshold value"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::getThreshold
PERSALYS_LimitState_getThreshold_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getPythonScript_doc
"Python script for limit state accessor.

Returns
-------
script : str
    Python script to rebuild the limit state"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::getPythonScript
PERSALYS_LimitState_getPythonScript_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getPhysicalModel_doc
"Physical model for limit state accessor.

Returns
-------
model : :class:`~persalys.PhysicalModel`
    Physical model"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::getPhysicalModel
PERSALYS_LimitState_getPhysicalModel_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOperator_doc
"Comparison operator accessor.

Parameters
----------
operator : :py:class:`openturns.ComparisonOperator`
    Comparison operator"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::setOperator
PERSALYS_LimitState_setOperator_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getOperator_doc
"Comparison operator accessor.

Returns
-------
operator : :py:class:`openturns.ComparisonOperator`
    Comparison operator"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::getOperator
PERSALYS_LimitState_getOperator_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_isValid_doc
"Whether limit state is valid.

Returns
-------
isValid : bool
    Whether it is valid"
%enddef
%feature("docstring") PERSALYS::LimitStateImplementation::isValid
PERSALYS_LimitState_isValid_doc
