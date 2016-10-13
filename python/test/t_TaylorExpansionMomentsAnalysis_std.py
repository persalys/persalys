#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

X0 = otguibase.Input('X0', 1, '', ot.Normal(1, 1))
X1 = otguibase.Input('X1', 2, '', ot.Normal(1, 1))
Y0 = otguibase.Output('Y0', 0, '', 'sin(X0) + 8*X1')

model = otguibase.AnalyticalPhysicalModel('aModelPhys', [X0, X1], [Y0])
myStudy.add(model)

## Taylor Expansion ##
analysis = otguibase.TaylorExpansionMomentsAnalysis('aTaylor', model)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)