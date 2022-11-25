#! /usr/bin/env python

import openturns as ot
import persalys
# from openturns.viewer import View

myStudy = persalys.Study("myStudy")

# Model
X0 = persalys.Input("X0", 0)
X1 = persalys.Input("X1", 0)

Y0 = persalys.Output("Y0")
Y1 = persalys.Output("Y1")

bounds = ot.Interval([0.0, 0.0], [5.0, 5.0])
model = persalys.SymbolicPhysicalModel(
    "aModelPhys", [X0, X1], [Y0, Y1],
    ["X0", "var g := 1.0 + 9.0 * (X0 + X1); g * (1.0 - sqrt(X0 / g))"])
myStudy.add(model)

# Optim #
ngen = 10
analysis = persalys.MultiObjectiveOptimizationAnalysis("optim", model, "nsga2")
analysis.setInterestVariables(["Y0", "Y1"])
analysis.addConstraint("Y0<3.")
analysis.setBounds(bounds)
analysis.setPopulationSize(50)
analysis.setGenerationNumber(ngen)
analysis.setVariableInputs(["X0", "X1"])
types = [ot.OptimizationProblemImplementation.CONTINUOUS,
         ot.OptimizationProblemImplementation.INTEGER]
analysis.setVariablesType(types)
myStudy.add(analysis)
analysis.run()
print("analysis=", analysis)
results = analysis.getResult()
print("results=", results)
print("final population=", results.getFinalPop())

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
