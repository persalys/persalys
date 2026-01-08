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

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::getNotEvaluatedInputSample
"Not evaluated input sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Points of the design of experiments which were not evaluated"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::setDesignOfExperiment
"Set the design of experiments.
Parameters
----------
designOfExperiment : :class:`~persalys.DesignOfExperiment`
    Design of experiments to set"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::getResult
"Get the data analysis result.

Returns
-------
result : :class:`~persalys.DataAnalysisResult`
    Data analysis result associated to the design of experiments"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::checkAndSetEvaluations
"Checks that the input values of the given DOE 
correspond to the current DOE and copy the evaluations 
of the given DOE to the current DOE

Parameters
----------
evaluatedDoE : :py::class:`~persalys.DesignOfExperiment`
    Design of experiments containing evaluations to copy"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::DesignOfExperimentEvaluation::setEvaluations
"Add evaluations for the design of experiments

Parameters
----------
outputSample : :py::class:`openturns.Sample`
    sample containing values for the output variables"