#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
X0 = otguibase.Input('X0', 1, '')
X1 = otguibase.Input('X1', 2, '')
Y0 = otguibase.Output('Y0', 0, 'output_1')
inputs = [X0, X1]
outputs = [Y0]
formulas = ['sin(X0)+8*X1']
model = otguibase.AnalyticalPhysicalModel('aModelPhys', inputs, outputs, formulas)
myStudy.add(model)
print(model)

function = model.getFunction()
print('function=', function)

formulas = model.getFormulas()
print('formulas=', formulas)

formula_Y0 = model.getFormula('Y0')
print('formula_Y0=', formula_Y0)

model.setFormulas(['sin(X0)+8*X1+0.5'])
print('formulas=', model.getFormulas())

## script
script = model.getPythonScript()
print('script=', script)
exec(script)
