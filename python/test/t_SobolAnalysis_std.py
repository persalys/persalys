#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
from math import pi

myStudy = otguibase.OTStudy('myStudy')

## Model
x0 = otguibase.Input('x0', 0., '', ot.Normal())
x1 = otguibase.Input('x1', 0., '', ot.Normal())
y0 = otguibase.Output('y0', 0., '', '10+3*x0+x1')

model = otguibase.AnalyticalPhysicalModel('aModel', [x0, x1], [y0])
myStudy.add(model)

## Sobol ##
analysis = otguibase.SobolAnalysis('aSobol', model)
analysis.setMaximumCalls(1000)
analysis.setMaximumCoefficientOfVariation(-1)
analysis.setMaximumElapsedTime(30)
analysis.setSeed(2)
analysis.setBlockSize(1000)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## Sobol ##
analysis2 = otguibase.SobolAnalysis('aSobol2', model)
analysis2.setMaximumCoefficientOfVariation(0.02)
analysis2.setMaximumElapsedTime(100000)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)