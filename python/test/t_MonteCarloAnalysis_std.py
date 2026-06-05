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

# Monte Carlo - test allowFailedEvaluations ##

X3 = persalys.Input("X3", ot.Normal(1, 1))
Y3 = persalys.Output("Y3")

failCode = """def _exec(X3):
    if X3 < 0.5:
        raise ValueError('X3 is too small')
    Y3 = X3
    return Y3
"""
failModel = persalys.PythonPhysicalModel("failModel", [X3], [Y3], failCode)
failModel.setParallel(False)

# allowFailedEvaluations=False: analysis stops at first error
analysis5 = persalys.MonteCarloAnalysis("myMonteCarlo5", failModel)
analysis5.setMaximumCalls(1000)
analysis5.setMaximumCoefficientOfVariation(-1)
analysis5.setSeed(0)
analysis5.setAllowFailedEvaluations(False)

analysis5.run()

# only 1 failed point was recorded (stopped at first failure)
print("failed (no allow):", analysis5.getFailedInputSample().getSize() == 1)
# less evaluations than requested
print("early stop:", analysis5.getResult().getDesignOfExperiment().getSample().getSize() < 1000)

# allowFailedEvaluations=True: analysis continues past errors
analysis6 = persalys.MonteCarloAnalysis("myMonteCarlo6", failModel)
analysis6.setMaximumCalls(1000)
analysis6.setMaximumCoefficientOfVariation(-1)
analysis6.setSeed(0)
analysis6.setAllowFailedEvaluations(True)

analysis6.run()

# multiple failed points recorded
print("failed (allow):", analysis6.getFailedInputSample().getSize() > 1)
# total evaluations = successful + failed = 1000
nSuccess = analysis6.getResult().getDesignOfExperiment().getSample().getSize()
nFailed = analysis6.getFailedInputSample().getSize()
print("total evals:", nSuccess + nFailed == 1000)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
