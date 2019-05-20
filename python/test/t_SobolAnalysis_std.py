#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import persalys
from math import pi

myStudy = persalys.Study('myStudy')

# Model
x0 = persalys.Input('x0', ot.Normal())
x1 = persalys.Input('x1', ot.Normal())
y00 = persalys.Output('fake_y0')
y00.setIsSelected(False)
y0 = persalys.Output('y0')

formula_y00 = 'x0'
formula_y0 = '10+3*x0+x1'
model = persalys.SymbolicPhysicalModel('aModel', [x0, x1], [y00, y0], [
                                        formula_y00, formula_y0])
myStudy.add(model)

# Sobol ##
analysis = persalys.SobolAnalysis('aSobol', model)
analysis.setMaximumCalls(1000)
analysis.setMaximumConfidenceIntervalLength(-1)
analysis.setMaximumElapsedTime(30)
analysis.setSeed(2)
analysis.setReplicationSize(250)
analysis.setBlockSize(4)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("result=", analysis.getResult())

# Sobol ##
analysis2 = persalys.SobolAnalysis('aSobol2', model)
analysis2.setMaximumConfidenceIntervalLength(0.2)
analysis2.setMaximumElapsedTime(100000)
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

print("result=", analysis2.getResult())

# Sobol ##
X2 = persalys.Input('x2', 10)
model.addInput(X2)
model.addOutput(persalys.Output('y1'))
model.setFormula('y1', '3*x0 + x1 + x2')

analysis3 = persalys.SobolAnalysis('aSobol3', model)
analysis3.setMaximumCalls(1000)
analysis3.setReplicationSize(200)
analysis3.setBlockSize(4)
analysis3.setSeed(2)
analysis3.setInterestVariables(['y1'])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
print("result=", result3)
persalys.Study.Add(myStudy)
# script
script = myStudy.getPythonScript()
print(script)
exec(script)
