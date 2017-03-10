#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
from math import pi

myStudy = otguibase.OTStudy('myStudy')

## Model
x0 = otguibase.Input('x0', ot.Normal())
x1 = otguibase.Input('x1', ot.Normal())
y00 = otguibase.Output('fake_y0')
y00.setIsSelected(False)
y0 = otguibase.Output('y0')

formula_y00 = 'x0'
formula_y0 = '10+3*x0+x1'
model = otguibase.AnalyticalPhysicalModel('aModel', [x0, x1], [y00, y0], [formula_y00, formula_y0])
myStudy.add(model)

## SRC ##
analysis = otguibase.SRCAnalysis('aSRC', model)
analysis.setSimulationsNumber(1000)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## SRC ##
X2 = otguibase.Input('x2', 10)
model.addInput(X2)
model.addOutput(otguibase.Output('y1'))
model.setFormula('y1', '3*x0 + x1 + x2')

analysis2 = otguibase.SRCAnalysis('aSRC2', model)
analysis2.setSimulationsNumber(1000)
analysis2.setSeed(2)
analysis2.setInterestVariables(['y1'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

result2 = analysis2.getResult()
print("result=", result2)

## script
script = myStudy.getPythonScript()
print(script)
exec(script)