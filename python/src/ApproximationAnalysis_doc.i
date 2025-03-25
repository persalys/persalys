%feature("docstring") PERSALYS::ApproximationAnalysis
"Class to define reliability analysis using approximation method.

"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ApproximationAnalysis::getPhysicalStartingPoint
"Physical starting point accessor.

Returns
-------
point : :py:class:`openturns.Point`
    Physical starting point. By default it set to the mean of the composed
    distribution of the physical model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ApproximationAnalysis::setPhysicalStartingPoint
"Physical starting point accessor.

Parameters
----------
point : sequence of float
    Physical starting point. By default it set to the mean of the composed
    distribution of the physical model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ApproximationAnalysis::getOptimizationAlgorithm
"Accessor to the optimization algorithm used to find the design point.

Returns
-------
algorithm : :py:class:`openturns.OptimizationAlgorithm`
    Optimization algorithm used to research the design point. Cobyla is
    the default used algorithm."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ApproximationAnalysis::setOptimizationAlgorithm
"Accessor to the optimization algorithm used to find the design point.

Parameters
----------
algorithm : :py:class:`openturns.OptimizationAlgorithm`
    Optimization algorithm used to research the design point. Cobyla is
    the default used algorithm."


