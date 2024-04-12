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
formula_Y0 = "sin(X0) + 8*X1"
model = persalys.SymbolicPhysicalModel(
    "aModelPhys", [X0, X1], [Y00, Y0], [formula_Y00, formula_Y0]
)
myStudy.add(model)

# Monte Carlo ##
analysis = persalys.MonteCarloAnalysis("myMonteCarlo", model)
analysis.setLevelConfidenceInterval(0.93)
analysis.setMaximumCalls(1000)
analysis.setMaximumCoefficientOfVariation(-1)
analysis.setMaximumElapsedTime(30)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print("PDF=", result.getPDF())
print("CDF=", result.getCDF())
print("outliers=", result.getOutliers())

# Monte Carlo ##
analysis2 = persalys.MonteCarloAnalysis("myMonteCarlo2", model)
analysis2.setIsConfidenceIntervalRequired(False)
analysis2.setMaximumCoefficientOfVariation(0.02)
analysis2.setMaximumElapsedTime(100000)
analysis2.setBlockSize(100)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

result2 = analysis2.getResult()
print("result=", result2)
print("PDF=", result2.getPDF())
print("CDF=", result2.getCDF())
print("outliers=", result2.getOutliers())

# Monte Carlo ##
X2 = persalys.Input("X2", 2)
model.addInput(X2)
model.addOutput(persalys.Output("Y1"))
model.setFormula("Y1", "sin(X0) + 8*X1 + X2")

analysis3 = persalys.MonteCarloAnalysis("myMonteCarlo3", model)
analysis3.setMaximumCalls(1000)
analysis3.setInterestVariables(["Y1"])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
print("result=", result3)

# Monte Carlo - test max CI length stopping criterion ##

X2 = persalys.Input("X2", ot.Normal(0, 1))
Y2 = persalys.Output("Y2")
formula_Y2 = "X2*X2"

model2 = persalys.SymbolicPhysicalModel("aModelPhys2", [X2], [Y2], [formula_Y2])
myStudy.add(model2)

analysis4 = persalys.MonteCarloAnalysis("myMonteCarlo4", model2)
analysis4.setMaximumConfidenceIntervalLength(0.1)
analysis4.setInterestVariables(["Y2"])
myStudy.add(analysis4)
print(analysis4)

analysis4.run()

result4 = analysis4.getResult()
print("result=", result4)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
