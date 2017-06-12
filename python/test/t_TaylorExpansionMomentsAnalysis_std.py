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
model = otguibase.SymbolicPhysicalModel('aModelPhys', [X0, X1], [Y00, Y0], [formula_Y00, formula_Y0])
myStudy.add(model)

## Taylor Expansion ##
analysis = otguibase.TaylorExpansionMomentsAnalysis('aTaylor', model)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())
print("\n")

## Taylor Expansion ##
# test with a deterministic variable
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
print("\n")

## Taylor Expansion ##
# test definition gradient with PythonPhysicalModel
code = 'from math import sin\n\ndef _exec(X0, X1):\n    Y0 = sin(X0) + 8*X1\n    fake_Y0 = X0\n    return Y0, fake_Y0\n'
model2 = otguibase.PythonPhysicalModel('myPhysicalModel', [X0, X1], [Y0], code)
myStudy.add(model2)

# default finite difference steps
analysis3 = otguibase.TaylorExpansionMomentsAnalysis('aTaylor3', model2)
analysis3.setInterestVariables(['Y0'])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

print("result=", analysis3.getResult())
print("\n")

# greater finite difference steps
model2.setFiniteDifferenceStep('X0', 0.005)
model2.setFiniteDifferenceStep('X1', 0.005)

analysis4 = otguibase.TaylorExpansionMomentsAnalysis('aTaylor4', model2)
analysis4.setInterestVariables(['Y0'])
myStudy.add(analysis4)
print(analysis4)

analysis4.run()

print("result=", analysis4.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)
