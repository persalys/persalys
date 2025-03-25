%feature("docstring") PERSALYS::FixedDesignOfExperiment
"Create a fixed design of experiments.

Available constructors:
    FixedDesignOfExperiment(*name, physicalModel*)

    FixedDesignOfExperiment(*name, physicalModel, sample*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
sample : :py:class:`openturns.Sample`
    Input sample. The sample must have a dimension equal to the number of
    inputs in the physical model (optional)

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the design of experiments:

>>> myDOE = persalys.FixedDesignOfExperiment('myDOE', myPhysicalModel)"


// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::FixedDesignOfExperiment::setOriginalInputSample
"Input sample accessor.

Parameters
----------
sample : :py:class:`openturns.Sample`
    Input sample. The sample must have a dimension equal to the number of
    inputs in the physical model"
