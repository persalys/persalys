%feature("docstring") PERSALYS::SimulationReliabilityResult
"Get simulation results from a reliability analysis using Monte Carlo or Importance Sampling method."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getSimulationResult
"Simulation result accessor.

Returns
-------
sample : :py:class:`openturns.SimulationResult`
    Simulation result"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getOutputSample
"Output sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Output sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getConvergenceSample
"Convergence sample accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Convergence sample"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getConvergenceSampleLowerBound
"Convergence sample lower bound accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Convergence sample lower bound"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getConvergenceSampleUpperBound
"Convergence sample upper bound accessor.

Returns
-------
sample : :py:class:`openturns.Sample`
    Convergence sample upper bound"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SimulationReliabilityResult::getElapsedTime
"Elapsed time in seconds to perform the analysis.

Returns
-------
time : int
    Elapsed time in seconds to perform the analysis"
