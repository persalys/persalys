%feature("docstring") PERSALYS::MultiObjectiveOptimizationAnalysis
"Perform the multi-objective optimization analysis of a model.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
solverName : str
    Optimization algorithm name (optional). Use :func:`GetSolverNames()` to list available names.

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> X0 = persalys.Input('X0', 0)
>>> X1 = persalys.Input('X1', 0)
>>> Y0 = persalys.Output('Y0')
>>> Y1 = persalys.Output('Y1')
>>> bounds = ot.Interval([0.0, 0.0], [5.0, 5.0])
>>> model = persalys.SymbolicPhysicalModel('aModelPhys', [X0, X1], [Y0, Y1], ['X0', 'var g := 1.0 + 9.0 * (X0 + X1); g * (1.0 - sqrt(X0 / g))'])

Create the analysis:

>>> analysis = persalys.MultiObjectiveOptimizationAnalysis('anAnalysis', model, 'nsga2')
>>> analysis.setInterestVariables(['Y0', 'Y1'])
>>> analysis.setBounds(bounds)
>>> analysis.setPopulationSize(50)
>>> analysis.setGenerationNumber(10)
>>> analysis.setVariableInputs(['X0', 'X1'])
>>> analysis.run() # doctest: +SKIP

Get the result:

>>> result = analysis.getResult() # doctest: +SKIP
>>> finalPop = result.getFinalPop() # doctest: +SKIP
>>> fronts = result.getFronts() # doctest: +SKIP"


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::getGenerationNumber
"Accessor to the number of generations.

Returns
-------
generationNumber : int
    number of generations for the evolving algorithm."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::setGenerationNumber
"Accessor to the number of generations.

Parameters
----------
generationNumber : int
    number of generations for the evolving algorithm."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::getSeed
"Accessor to random generator seed.

Returns
-------
seed : int
    random generator seed for the evolving algorithm."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::setSeed
"Accessor to random generator seed.

Parameters
----------
seed : int
    random generator seed for the evolving algorithm."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::getPopulationSize
"Accessor to popSize.

Returns
-------
popSize : int
    Initial population sample size."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::setPopulationSize
"Accessor to popSize.

Parameters
----------
popSize : int
    Initial population sample size."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::OptimizationAnalysis::getResult
"Accessor to result.

Returns
-------
result : :class:`~persalys.MultiObjectiveOptimizationAnalysisResult`
    multi-objective analysis result."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MultiObjectiveOptimizationAnalysisResult
"Multi-objective optimization analysis result."

%feature("docstring") PERSALYS::MultiObjectiveOptimizationAnalysisResult::getFinalPop
"Accessor to final population.

Parameters
----------
finalPop : :class:`~openturns.Sample`
    Final population sample. Final points (Xi) and values (Yi) are stacked together with an additional column containing the front index of each point.

+----+----+---------------+
| Xi | Yi | _front_index_ |
+----+----+---------------+
|    |    |               |"

%feature("docstring") PERSALYS::MultiObjectiveOptimizationAnalysisResult::getFronts
"Accessor to fronts data.

Parameters
----------
finalPop : Collection of :class:`~openturns.Sample`
    Collection of fronts sample. Each collection item contains final points (Xi). The i^{th} items contains points from front_i."
