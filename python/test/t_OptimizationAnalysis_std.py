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


# Model 2
X2 = persalys.Input("X2", ot.Normal(1, 1))
Y00 = persalys.Output("X2")
code = "def _exec(X0,X1,X2):\n    Y00 = X0\n    Y0 = 3*(X0-5)**2 + 8*(X1+8)**2\n    return Y0,X2"
model2 = persalys.PythonPhysicalModel("pythonModel", [X0, X1, X2], [Y0, Y00], code)
myStudy.add(model2)

analysis = persalys.OptimizationAnalysis("optim", model2, "Cobyla")
analysis.setInterestVariables(["Y0"])
analysis.setStartingPoint([0.5, 0.5, 0])
analysis.addConstraint("Y0<600.0")
interval = ot.Interval([-5, 0, -2], [5, 0, 2])
analysis.setBounds(interval)
analysis.setVariableInputs(["X0"])
analysis.setMaximumEvaluationNumber(150)
analysis.setMaximumAbsoluteError(1e-6)
analysis.setMaximumRelativeError(1e-6)
analysis.setMaximumResidualError(1e-6)
analysis.setMaximumConstraintError(1e-6)
analysis.run()
cstr = analysis.defineProblem().getInequalityConstraint()
point = analysis.getResult().getFinalPoints()
print(cstr(point))
