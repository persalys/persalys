%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment
"Create a probabilistic design of experiments.

Available constructors:
    ProbabilisticDesignOfExperiment(*name, physicalModel*)

    ProbabilisticDesignOfExperiment(*name, physicalModel, size, designName*)

Parameters
----------
name : str
    Name
physicalModel : :class:`~persalys.PhysicalModel`
    Physical model with at least a stochastic input variable.
size : positive int
    Number of points in the design.
designName : str
    Name of the design. Use :func:`GetDesignNames()` to list available names.

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

>>> myDOE = persalys.ProbabilisticDesignOfExperiment('myDOE', myPhysicalModel, 10, 'MONTE_CARLO')"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::setDesignName
"Accessor to the design name.

Parameters
----------
designName : str
    The design name. Use :func:`GetDesignNames()` to list available names."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::getDesignName
"Accessor to the design name.

Returns
-------
designName : str
    The design name."

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::GetDesignNames
"Accessor to the list of designs provided by ProbabilisticDesignOfExperiment.

Returns
-------
names : :py:class:`openturns.Description`
    List of design names provided by ProbabilisticDesignOfExperiment.

Examples
--------
>>> import persalys
>>> print(persalys.ProbabilisticDesignOfExperiment.GetDesignNames())
[LHS,SALHS,MCLHS,MONTE_CARLO,QUASI_MONTE_CARLO]
"

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::setSeed
"Initialize the random generator seed.

Parameters
----------
seed : int
    Seed value."

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::getSeed
"Random generator seed accessor.

Returns
-------
seed : int
    Seed value."

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::setSize
"Accessor to the size of the generated sample.

Parameters
----------
size : positive int
    Number of points in the design."

// ----------------------------------------------------------------------

%feature("docstring") PERSALYS::ProbabilisticDesignOfExperiment::getSize
"Accessor to the size of the generated sample.

Returns
-------
size : positive int
    Number of points in the design."
