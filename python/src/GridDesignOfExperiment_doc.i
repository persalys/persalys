%feature("docstring") PERSALYS::GridDesignOfExperiment
"Create a grid design of experiments.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
values : sequence of :py:class:`openturns.Point`
    Values of the variables (optional)

Notes
-----
The length of value must be equal to the number of inputs of the physical model.
The values can be obtained using class :py:class:`openturns.Box` for example.

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

>>> levels = [3, 4]
>>> bounds = [[200., 60000.], [400., 90000.]]
>>> values = [ot.Box([levels[i] - 2], ot.Interval(bounds[i][0], bounds[i][1])).generate().asPoint() for i in range(len(levels))]
>>> myDOE = persalys.GridDesignOfExperiment('myDOE', myPhysicalModel, values)"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getValues
"Values accessor.

Returns
-------
values : sequence of :py:class:`openturns.Point`
   Inputs values used in the case where there is at least a constant variable."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::setValues
"Values accessor.

Parameters
----------
values : sequence of :py:class:`openturns.Point`
   Inputs values used in the case where there is at least a constant variable."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::GetDefaultBounds
"Get the default bounds for the grid design of experiments.

Parameters
----------
model : :class:`~persalys.PhysicalModel`
   Physical model

Returns
-------
bounds : :py:class:`openturns.Interval`
   Default bounds"
