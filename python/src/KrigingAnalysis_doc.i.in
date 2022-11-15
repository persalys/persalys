%feature("docstring") PERSALYS::KrigingAnalysis
"Create a Kriging analysis.

See `Kriging <http://openturns.github.io/openturns/latest/theory/meta_modeling/kriging.html>`_.

Parameters
----------
name : str
    Name
designOfExperiment : :class:`~persalys.DesignOfExperiment`
    Design of experiments

Examples
--------
Create the model:

>>> import openturns as ot
>>> import persalys
>>> from math import pi
>>> ot.RandomGenerator.SetSeed(0)

Create the model:

>>> xi1 = persalys.Input('xi1', ot.Uniform(-pi, pi))
>>> xi2 = persalys.Input('xi2', ot.Uniform(-pi, pi))
>>> xi3 = persalys.Input('xi3', ot.Uniform(-pi, pi))
>>> y0 = persalys.Output('y0')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [xi1, xi2, xi3], [y0], ['sin(xi1) + 7. * (sin(xi2)) ^ 2 + 0.1 * xi3^4 * sin(xi1)'])

Create the design of experiments:

>>> aDesign = persalys.ProbabilisticDesignOfExperiment('aDesign', myPhysicalModel, 100, 'LHS')
>>> aDesign.run()

Create the Kriging Analysis:

>>> kriging = persalys.KrigingAnalysis('kriging', aDesign)
>>> kriging.setBasis(ot.LinearBasisFactory(3).build())
>>> kriging.setCovarianceModel(ot.GeneralizedExponential(3))
>>> kriging.setLeaveOneOutValidation(False)
>>> kriging.run()

Get the result:

>>> krigingResult = kriging.getResult()"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::getCovarianceModel
"Covariance model accessor.

Returns
-------
model : :py:class:`openturns.CovarianceModel`
    Covariance model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::setCovarianceModel
"Covariance model accessor.

Parameters
----------
model : :py:class:`openturns.CovarianceModel`
    Covariance model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::getBasis
"Trend basis accessor.

Returns
-------
basis : :py:class:`openturns.Basis`
    Trend basis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::setBasis
"Trend basis accessor.

Parameters
----------
basis : :py:class:`openturns.Basis`
    Trend basis"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::getOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Returns
-------
optimize : bool
    Whether to optimize the covariance model parameters"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::setOptimizeParameters
"Accessor to the covariance model parameters optimization flag.

Parameters
----------
optimize : bool
    Whether to optimize the covariance model parameters"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::KrigingAnalysis::getResult
"Results accessor.

Returns
-------
result : :class:`~persalys.KrigingAnalysisResult`
    Results"
