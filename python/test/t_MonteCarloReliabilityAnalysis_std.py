#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
X0 = otguibase.Input('X0', 1, '', ot.Normal(1, 1))
X1 = otguibase.Input('X1', 2, '', ot.Normal(1, 1))
Y0 = otguibase.Output('Y0', 0, '', 'sin(X0) + 8*X1')

model = otguibase.AnalyticalPhysicalModel('aModelPhys', [X0, X1], [Y0])
myStudy.add(model)

## limit state ##
limitState = otguibase.LimitState('aLimitState', model, 'Y0', ot.Greater(), 20.)
myStudy.add(limitState)

## Monte Carlo ##
analysis = otguibase.MonteCarloReliabilityAnalysis('myMonteCarlo', limitState)
analysis.setMaximumCalls(1000)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## Monte Carlo ##
analysis2 = otguibase.MonteCarloReliabilityAnalysis('myMonteCarlo2', limitState)
analysis2.setMaximumCoefficientOfVariation(0.02)
analysis2.setMaximumElapsedTime(100000)
analysis2.setBlockSize(100)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)