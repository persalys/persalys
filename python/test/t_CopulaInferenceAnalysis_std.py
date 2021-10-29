#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os

myStudy = persalys.Study('myStudy')

# Model
filename = 'data2.csv'
ot.RandomGenerator.SetSeed(0)
sample = ot.Normal(3).getSample(300)
sample.stack(ot.Gumbel().getSample(300))
sample.setDescription(['X0', 'X1', 'X2', 'X3'])
sample.exportToCSVFile(filename, ',')
columns = [0, 2, 3]

model = persalys.DataModel('myDataModel', "data2.csv", columns)
myStudy.add(model)
print(model)

# Dependencies inference analysis ##
analysis = persalys.CopulaInferenceAnalysis('analysis', model)
variables = ["X0", "X3"]
factories = [ot.NormalCopulaFactory(), ot.GumbelCopulaFactory()]
analysis.setDistributionsFactories(variables, factories)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print(result.getCopulaInferenceSetResult(variables))

# script
script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
