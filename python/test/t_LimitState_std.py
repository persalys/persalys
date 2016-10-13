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
print(limitState)
myStudy.add(limitState)

limitState.setThreshold(10.)
limitState.setOperator(ot.Less())
print(limitState)

## limit state ##
limitState2 = otguibase.LimitState('aLimitState2', model)
print(limitState2)
myStudy.add(limitState2)

limitState2.setThreshold(15.)
print(limitState2)

model.addOutput(otguibase.Output('Y1', 0, '', '1 + sin(X0) + 8*X1'))
limitState2.setOutputName('Y1')
print(limitState2)

## script
script = myStudy.getPythonScript()
print(script)
exec(script)