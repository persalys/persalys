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

## SRC ##
analysis = otguibase.SRCAnalysis('aSRC', model)
analysis.setSimulationsNumber(1000)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)