%feature("docstring") PERSALYS::ModelEvaluation
"Generate a simple evaluation of a model.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
values : :py:class:`openturns.Point`
    Input values (optional)

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the Model evaluation:

>>> analysis = persalys.ModelEvaluation('anAnalysis', myPhysicalModel)
>>> analysis.run()

Get the result:

>>> outputValues = analysis.getResult().getDesignOfExperiment().getOutputSample()"
