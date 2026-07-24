#! /usr/bin/env python

import openturns as ot
import persalys

myStudy = persalys.Study("myStudy")

# Model
x0 = persalys.Input("x0", ot.Normal())
x1 = persalys.Input("x1", ot.Normal())
y00 = persalys.Output("fake_y0")
y00.setIsSelected(False)
y0 = persalys.Output("y0")

formula_y00 = "x0"
formula_y0 = "10+3*x0+x1"
model = persalys.SymbolicPhysicalModel(
    "aModel", [x0, x1], [y00, y0], [formula_y00, formula_y0]
)
myStudy.add(model)

# Sobol ##
analysis = persalys.SobolAnalysis("aSobol", model)
analysis.setMaximumCalls(1000)
analysis.setMaximumConfidenceIntervalLength(-1)
analysis.setMaximumElapsedTime(30)
analysis.setSeed(2)
analysis.setReplicationSize(250)
analysis.setBlockSize(4)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

# Sobol ##
analysis2 = persalys.SobolAnalysis("aSobol2", model)
analysis2.setMaximumConfidenceIntervalLength(0.2)
analysis2.setMaximumElapsedTime(100000)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

# Sobol ##
X2 = persalys.Input("x2", 10)
model.addInput(X2)
model.addOutput(persalys.Output("y1"))
model.setFormula("y1", "3*x0 + x1 + x2")

analysis3 = persalys.SobolAnalysis("aSobol3", model)
analysis3.setMaximumCalls(1000)
analysis3.setReplicationSize(200)
analysis3.setBlockSize(4)
analysis3.setSeed(2)
analysis3.setInterestVariables(["y1"])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
print("result=", result3)

# Sobol - test BatchFailedException handling ##

x0_fail = persalys.Input("x0_fail", ot.Normal())
x1_fail = persalys.Input("x1_fail", ot.Normal())
y0_fail = persalys.Output("y0_fail")

failCode = """def _exec(x0_fail, x1_fail):
    if abs(x0_fail) >= 1.9:
        raise ValueError('x0_fail is too large')
    y0_fail = x0_fail + x1_fail
    return y0_fail
"""
failModel = persalys.PythonPhysicalModel("failModel", [x0_fail, x1_fail], [y0_fail], failCode)
failModel.setParallel(False)

analysis4 = persalys.SobolAnalysis("aSobol4", failModel)
analysis4.setSeed(11)
analysis4.setReplicationSize(3)
analysis4.setBlockSize(6)

analysis4.run()

result4 = analysis4.getResult()
# the run stopped on a BatchFailedException: a warning message is set
assert len(analysis4.getWarningMessage()) > 0

# the points that succeeded, even within the batch that raised the exception, were recovered
doe4 = result4.getDesignOfExperiment()
assert doe4.getSample().getSize() == 65

# indices could not be computed since the algorithm stopped on the exception
assert result4.getFirstOrderIndices().getSize() == 0
assert result4.getTotalIndices().getSize() == 0

persalys.Study.Add(myStudy)
# script
script = myStudy.getPythonScript()
print(script)
exec(script)
