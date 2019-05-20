#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import persalys

myStudy = persalys.Study('myStudy')

# Model
X0 = persalys.Input('X0', ot.Normal(1, 1))
X1 = persalys.Input('X1', ot.Normal(1, 1))
Y0 = persalys.Output('Y0')

model = persalys.SymbolicPhysicalModel(
    'aModelPhys', [X0, X1], [Y0], ['sin(X0) + 8*X1'])
myStudy.add(model)

# limit state ##
limitState = persalys.LimitState(
    'aLimitState', model, 'Y0', ot.Greater(), 20.)
myStudy.add(limitState)

# FORM ##
analysis = persalys.FORMAnalysis('myFORM', limitState)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult().getFORMResult())

# FORM ##
X2 = persalys.Input('X2', 2)
model.addInput(X2)
model.setFormula('Y0', 'sin(X0) + 8*X1 + X2')

analysis2 = persalys.FORMAnalysis('myFORM2', limitState)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

result2 = analysis2.getResult().getFORMResult()
print("result=", result2)

# FORM ##
analysis3 = persalys.FORMAnalysis('myFORM3', limitState)
analysis3.setPhysicalStartingPoint([1.08161, 2.38966])
analysis3.setOptimizationAlgorithm(ot.AbdoRackwitz())
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult().getFORMResult()
print("result=", result3)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
