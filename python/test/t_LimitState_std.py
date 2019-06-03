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
print(limitState)
myStudy.add(limitState)

limitState.setThreshold(10.)
limitState.setOperator(ot.Less())
print(limitState)

# limit state ##
limitState2 = persalys.LimitState('aLimitState2', model)
print(limitState2)
myStudy.add(limitState2)

limitState2.setThreshold(15.)
print(limitState2)

model.addOutput(persalys.Output('Y1'))
model.setFormula('Y1', '1 + sin(X0) + 8*X1')
limitState2.setOutputName('Y1')
print(limitState2)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
