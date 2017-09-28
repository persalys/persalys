#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

# Model
X0 = otguibase.Input('X0', ot.Normal(1, 1))
X1 = otguibase.Input('X1', ot.Normal(1, 1))
Y0 = otguibase.Output('Y0')

model = otguibase.SymbolicPhysicalModel(
    'aModelPhys', [X0, X1], [Y0], ['sin(X0) + 8*X1'])
myStudy.add(model)

# limit state ##
limitState = otguibase.LimitState(
    'aLimitState', model, 'Y0', ot.Greater(), 20.)
myStudy.add(limitState)

# Monte Carlo ##
analysis = otguibase.MonteCarloReliabilityAnalysis('myMonteCarlo', limitState)
analysis.setMaximumCalls(1000)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

# Monte Carlo ##
analysis2 = otguibase.MonteCarloReliabilityAnalysis(
    'myMonteCarlo2', limitState)
analysis2.setMaximumCoefficientOfVariation(0.02)
analysis2.setMaximumElapsedTime(100000)
analysis2.setBlockSize(100)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

# Monte Carlo ##
X2 = otguibase.Input('X2', 2)
model.addInput(X2)
model.setFormula('Y0', 'sin(X0) + 8*X1 + X2')

analysis3 = otguibase.MonteCarloReliabilityAnalysis(
    'myMonteCarlo3', limitState)
analysis3.setMaximumCalls(1000)
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
print("result=", result3)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
