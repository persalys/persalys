%feature("docstring") PERSALYS::SymbolicPhysicalModel
"Create a physical model defined by analytical formulas.

Parameters
----------
name : str
    Name
inputs : sequence of :class:`~persalys.Input`
    Input variables (optional)
outputs : sequence of :class:`~persalys.Output`
    Output variables (optional)
formulas : sequence of str
    List of analytical formulas between the inputs and the outputs (optional).
    The function is defined by outputs = formulas(inputs).

Examples
--------
>>> import openturns as ot
>>> import persalys

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> model = persalys.SymbolicPhysicalModel('myPhysicalModel', [R, F], [G], ['R-F/(pi_*100.0)'])

>>> y = model.getFunction()([300.0,75000.0])"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SymbolicPhysicalModel::getFormulas
"Formulas accessor.

Returns
-------
formulas : :py:class:`openturns.Description`
    Output formulas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SymbolicPhysicalModel::setFormulas
"Formulas accessor.

Parameters
----------
formulas : sequence of str
    Output formulas"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SymbolicPhysicalModel::getFormula
"Output formula accessor.

Parameters
----------
name : str
    Variable name

Returns
-------
formula : str
    Formula text"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::SymbolicPhysicalModel::setFormula
"Output formula accessor.

Parameters
----------
name : str
    Variable name
formula : str
    Formula text"
