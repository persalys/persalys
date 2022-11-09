#! /usr/bin/env python

import openturns as ot
import persalys

myStudy = persalys.Study("myStudy")

# Model
X0 = persalys.Input("X0", ot.Normal(1, 1))
X1 = persalys.Input("X1", ot.Normal(1, 1))
Y00 = persalys.Output("fake_Y0")
Y00.setIsSelected(False)
Y0 = persalys.Output("Y0")

formula_Y00 = "X0"
formula_Y0 = "3*(X0-5)^2 + 8*(X1+8)^2"
model = persalys.SymbolicPhysicalModel(
    "aModelPhys", [X0, X1], [Y00, Y0], [formula_Y00, formula_Y0]
)
myStudy.add(model)

# Monte Carlo ##
analysis = persalys.OptimizationAnalysis("optim", model, "Cobyla")
analysis.setInterestVariables(["Y0"])
analysis.setStartingPoint([0.5, 0.5])
analysis.addConstraint("Y0<600.0")
interval = ot.Interval([-5, 0], [5, 0])
analysis.setBounds(interval)
analysis.setVariableInputs(["X0"])
analysis.setMaximumEvaluationNumber(150)
analysis.setMaximumAbsoluteError(1e-6)
analysis.setMaximumRelativeError(1e-6)
analysis.setMaximumResidualError(1e-6)
analysis.setMaximumConstraintError(1e-6)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
# print("result=", result)
print("x=", result.getOptimalPoint())
print("y=", result.getOptimalValue())

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
