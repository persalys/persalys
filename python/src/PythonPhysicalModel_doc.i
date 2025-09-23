%feature("docstring") PERSALYS::PythonPhysicalModel
R"RAW(Create a physical model defined by Python code.

Available constructors:
    PythonPhysicalModel(*name* = "Unnamed")
    PythonPhysicalModel(*name, inputs, outputs, code*)
    PythonPhysicalModel(*metaModelAnalysis, metaModel, study*)

Parameters
----------
name : str
    Name
inputs : sequence of :class:`~persalys.Input`
    Input variables
outputs : sequence of :class:`~persalys.Output`
    Output variables
code : str
    Code string
metaModelAnalysis : :class:`~persalys.MetaModelAnalysis`
    metamodel analysis to use inside the python model
metaModel : :class:`~persalys.PhysicalModel`
    The metamodel created by the metamodel analysis
    obtained by metaModelAnalysis.getImplementation().getResult().getMetaModel()
study : :class:`~persalys.Study`
    The study to which the metamodel analysis is attached.

Notes
-----
The code string must contain a function named '_exec' that takes float
arguments and return a float or a sequence of floats.

Examples
--------
>>> import openturns as ot
>>> import persalys

Create the model:

>>> R = persalys.Input('R', 0., ot.LogNormalMuSigma(300., 30., 0.).getDistribution(), 'Yield strength')
>>> F = persalys.Input('F', 0., ot.Normal(75000., 5000.), 'Traction load')
>>> G = persalys.Output('G', 'deviation')
>>> code = 'from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return G\n'
>>> model = persalys.PythonPhysicalModel('myPhysicalModel', [R, F], [G], code)
>>> y = model.getFunction()([300.0,75000.0])
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::PythonPhysicalModel::setCode
"Code accessor.

Parameters
----------
code : str
    Code text"

// ---------------------------------------------------------------------

%feature("docstring") PERSALYS::PythonPhysicalModel::getCode
"Code accessor.

Returns
-------
code : str
    Code text"
