%define PERSALYS_LimitState_doc
"Create a limit state.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
outputNames : str or collection of str
     Name(s) of output variable(s) of interest in limit state
operator : :py:class:`~openturns.ComparisonOperator` or collection of :py:class:`~openturns.ComparisonOperator`
    Comparison operator(s) against threshold(s) (optional)
threshold : float or :py:class:`~openturns.Point`
    Threshold value(s) (optional)
type : int
    Type of system limit state (optional, default is Union)
    Possible values are persalys.LimitState.Union and persalys.LimitState.Intersection.

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

%define PERSALYS_LimitState_getOutputNames_doc
"Output variable names accessor.

Returns
-------
outputNames : :py:class:`~openturns.Description`
    Output variable names"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getOutputNames
PERSALYS_LimitState_getOutputNames_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOutputNames_doc
"Set all output variable names.

Parameters
----------
outputNames : collection of str
    Output variable names"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setOutputNames
PERSALYS_LimitState_setOutputNames_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOutputName_doc
"Set one output variable name.

Parameters
----------
index : int
    Failure event index
outputName : str
    Output variable name"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setOutputName
PERSALYS_LimitState_setOutputName_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getOperators_doc
"Comparison operators accessor.

Returns
-------
operators : collection of :py:class:`~openturns.ComparisonOperator`
    Comparison operators"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getOperators
PERSALYS_LimitState_getOperators_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getOperator_doc
"Comparison operator accessor.

Parameters
----------
index : int
    Failure event index

Returns
-------
operator : :py:class:`openturns.ComparisonOperator`
    Comparison operator"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getOperator
PERSALYS_LimitState_getOperator_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOperators_doc
"Set all comparison operators.

Parameters
----------
operators : collection of :py:class:`~openturns.ComparisonOperator`
    Comparison operators"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setOperators
PERSALYS_LimitState_setOperators_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setOperator_doc
"Set one comparison operator.

Parameters
----------
index : int
    Failure event index
operator : :py:class:`openturns.ComparisonOperator`
    Comparison operator"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setOperator
PERSALYS_LimitState_setOperator_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getThresholds_doc
"Threshold values accessor.

Returns
-------
thresholds : :py:class:`~openturns.Point`
    Threshold values"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getThresholds
PERSALYS_LimitState_getThresholds_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getThreshold_doc
"Threshold value accessor.

Parameters
----------
index : int
    Failure event index

Returns
-------
threshold : float
    Threshold value"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getThreshold
PERSALYS_LimitState_getThreshold_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setThresholds_doc
"Set all threshold values.

Parameters
----------
thresholds : :py:class:`~openturns.Point`
    Threshold values"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setThresholds
PERSALYS_LimitState_setThresholds_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setThreshold_doc
"Set one threshold value.

Parameters
----------
index : int
    Failure event index
threshold : float
    Threshold value"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setThreshold
PERSALYS_LimitState_setThreshold_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_setType_doc
"Set the aggregation type of a system limit state.

Parameters
----------
type : int
    ``persalys.LimitState.Union`` or ``persalys.LimitState.Intersection``"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::setType
PERSALYS_LimitState_setType_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getType_doc
"Aggregation type accessor.

Returns
-------
type : int
    ``persalys.LimitState.Union`` or ``persalys.LimitState.Intersection``"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getType
PERSALYS_LimitState_getType_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_addFailureEvent_doc
"Add one failure event to the limit state.

Parameters
----------
variableName : str
    Variable name associated with the failure event
comparisonOperator : :py:class:`~openturns.ComparisonOperator`, optional
    Comparison operator
threshold : float, optional
    Threshold value"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::addFailureEvent
PERSALYS_LimitState_addFailureEvent_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_removeFailureEvent_doc
"Remove one failure event from the limit state.

Parameters
----------
index : int
    Failure event index"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::removeFailureEvent
PERSALYS_LimitState_removeFailureEvent_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_isSystemLimitState_doc
"Whether the limit state contains several failure events.

Returns
-------
isSystemLimitState : bool
    Whether the limit state is a system limit state"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::isSystemLimitState
PERSALYS_LimitState_isSystemLimitState_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getNumberOfFailureEvents_doc
"Get the number of failure events in the limit state.

Returns
-------
numberOfFailureEvents : int
    Number of failure events"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getNumberOfFailureEvents
PERSALYS_LimitState_getNumberOfFailureEvents_doc

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

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_getThresholdEvent_doc
"Get the threshold event corresponding to the limit state.

Returns
-------
event : :py:class:`~openturns.RandomVector`
    Threshold event"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::getThresholdEvent
PERSALYS_LimitState_getThresholdEvent_doc

// ---------------------------------------------------------------------

%define PERSALYS_LimitState_asComposedEvent_doc
"Get the the threshold event constructed specifically for importance sampling.

Returns
-------
event : :py:class:`~openturns.RandomVector`
    Threshold event"
%enddef

%feature("docstring") PERSALYS::LimitStateImplementation::asComposedEvent
PERSALYS_LimitState_asComposedEvent_doc

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
