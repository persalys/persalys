#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os

myStudy = persalys.Study('myStudy')

# Model
filename = 'data1.csv'
ot.RandomGenerator.SetSeed(0)
sample = ot.Normal(3).getSample(300)
sample.stack(ot.Gumbel().getSample(300))
sample.setDescription(['X0', 'X1', 'X2', 'X3'])
sample.exportToCSVFile(filename, ',')
columns = [0, 2, 3]

model = persalys.DataModel('myDataModel', "data1.csv", columns)
myStudy.add(model)
print(model)

# Inference analysis ##
analysis = persalys.InferenceAnalysis('analysis', model)
variables = ["X0", "X3"]
analysis.setInterestVariables(variables)
factories = [ot.NormalFactory(), ot.GumbelFactory()]
analysis.setDistributionsFactories("X3", factories)
analysis.setLevel(0.1)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print(result.getFittingTestResultForVariable('X3'))

# script
script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
