#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

X0 = otguibase.Input('X0', ot.Normal(1, 1))
X1 = otguibase.Input('X1', ot.Normal(1, 1))
Y00 = otguibase.Output('fake_Y0')
Y00.setIsSelected(False)
Y0 = otguibase.Output('Y0')

formula_Y00 = 'X0'
formula_Y0 = 'sin(X0) + 8*X1'
model = otguibase.AnalyticalPhysicalModel('aModelPhys', [X0, X1], [Y00, Y0], [formula_Y00, formula_Y0])
myStudy.add(model)

## Taylor Expansion ##
analysis = otguibase.TaylorExpansionMomentsAnalysis('aTaylor', model)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## Taylor Expansion ##
X2 = otguibase.Input('X2', 10)
model.addInput(X2)
model.addOutput(otguibase.Output('Y1'))
model.setFormula('Y1', 'sin(X0) + 8*X1 + X2')

analysis2 = otguibase.TaylorExpansionMomentsAnalysis('aTaylor2', model)
analysis2.setInterestVariables(['Y1'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)