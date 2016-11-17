#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase
import os

myStudy = otguibase.OTStudy('myStudy')

## Model
filename = 'data.csv'
ot.RandomGenerator_SetSeed(0)
sample = ot.Normal(3).getSample(300)
sample.stack(ot.Gumbel().getSample(300))
sample.setDescription(['X0', 'X1', 'X2', 'X3'])
sample.exportToCSVFile(filename, ',')
columns = [0, 2, 3]

model = otguibase.DataModel('myDataModel', "data.csv", columns)
myStudy.add(model)
print(model)

## Inference analysis ##
analysis = otguibase.InferenceAnalysis('analysis', model)
variables = ["X0", "X3"]
analysis.setInterestVariables(variables)
factories = [ot.NormalFactory(), ot.GumbelFactory()]
analysis.setDistributionsFactories("X3", factories)
analysis.setLevel(0.9)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print(result.getFittingTestResultForVariable('X3'))

## script
script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)