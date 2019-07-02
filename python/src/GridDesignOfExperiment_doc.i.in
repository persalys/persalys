%feature("docstring") PERSALYS::GridDesignOfExperiment
"Create a grid design of experiments.

Available constructors:
    GridDesignOfExperiment(*name, physicalModel*)

    GridDesignOfExperiment(*name, physicalModel, bounds, nbValues, values*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model
bounds : :py:class:`openturns.Interval`
    Bounds
nbValues : sequence of int
    Number of values along each direction
values : sequence of float
    Values of the constant variables (optional)

Notes
-----
The sequences must have a dimension equal to the number of inputs in the
physical model.

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

>>> myDOE = persalys.GridDesignOfExperiment('myDOE', myPhysicalModel)"


// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getTypeDesignOfExperiment
"Design type accessor.

Returns
-------
type : int
    Design type"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getValues
"Values accessor.

Returns
-------
values : :py:class:`openturns.Point`
   Inputs values used in the case where there is at least a constant variable."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::setValues
"Values accessor.

Parameters
----------
values : :py:class:`openturns.Point`
   Inputs values used in the case where there is at least a constant variable."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getBounds
"Bounds accessor.

Returns
-------
bounds : :py:class:`openturns.Interval`
   Bounds"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::setBounds
"Bounds accessor.

Parameters
----------
bounds : :py:class:`openturns.Interval`
   Bounds for each input variables of the model"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getLevels
"Levels accessor.

Returns
-------
levels : :py:class:`openturns.Indices`
   Levels"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::setLevels
"Levels accessor.

Parameters
----------
levels : :py:class:`openturns.Indices`
   Levels"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::getDeltas
"Deltas accessor.

Returns
-------
deltas : :py:class:`openturns.Point`
   Deltas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::GridDesignOfExperiment::setDeltas
"Deltas accessor.

Parameters
----------
deltas : :py:class:`openturns.Point`
   Deltas"
