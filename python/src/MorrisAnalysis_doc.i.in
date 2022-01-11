%feature("docstring") PERSALYS::MorrisAnalysis
"Perform a Morris analysis.

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> myPhysicalModel = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

Create the Morris analysis:

>>> analysis = persalys.MorrisAnalysis('anAnalysis', myPhysicalModel)
>>> analysis.setBounds(ot.Interval([100, 60000], [350, 80000]))
>>> analysis.setLevel(5)
>>> analysis.run() # doctest: +SKIP

Get the result:

>>> result = analysis.getResult() # doctest: +SKIP
>>> meanAbsEE = result.getMeanAbsoluteElementaryEffects(0) # doctest: +SKIP"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::getTrajectoriesNumber
"Number of trajectories accessor.

Returns
-------
number : int
    Number of trajectories. It is set by default to 10."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::setTrajectoriesNumber
"Number of trajectories accessor.

Parameters
----------
number : positive int
    Number of trajectories"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::getBounds
"Bounds of the domain accessor.

Returns
-------
bounds : :py:class:`openturns.Interval`
    Bounds of the domain"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::setBounds
"Bounds of the domain accessor.

Parameters
----------
bounds : :py:class:`openturns.Interval`
    Bounds of the domain"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::getLevel
"Level accessor.

Returns
-------
number : int
    Number of levels for a regular grid. It is set by default to 4."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::setLevel
"Level accessor.

Parameters
----------
number : positive int
    Number of levels for a regular grid"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::MorrisAnalysis::getResult
"Morris result accessor.

Returns
-------
result : :class:`~persalys.MorrisResult`
    Result"
