%feature("docstring") PERSALYS::DesignOfExperimentEvaluation
"Create the base class of design of experiments.
"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::getOriginalInputSample
"Input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Input sample."

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::getFailedInputSample
"Failed input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Points of the design of experiments which could not be evaluated"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::getNotEvaluatedInputSample
"Not evaluated input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Points of the design of experiments which were not evaluated"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::setEvaluations
"Add evaluations for the design of experiments

Parameters
----------
outputSample : :py::class:`openturns.Sample`
    sample containing values for the output variables"