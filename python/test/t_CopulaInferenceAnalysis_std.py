#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os

myStudy = persalys.Study("myStudy")

# Model
filename = "data2.csv"
ot.RandomGenerator.SetSeed(0)
R = ot.CorrelationMatrix(4)
R[0, 3] = 0.9
distribution = ot.JointDistribution([ot.Normal(), ot.Normal(), ot.Normal(), ot.Gumbel()], ot.NormalCopula(R))
sample = distribution.getSample(300)
sample.setDescription(["X0", "X1", "X2", "X3"])
sample.exportToCSVFile(filename, ",")
columns = [0, 2, 3]

model = persalys.DataModel("myDataModel", "data2.csv", columns)
myStudy.add(model)
print(model)

# Dependencies inference analysis ##
analysis = persalys.CopulaInferenceAnalysis("analysis", model)
variables = ["X0", "X3"]
factories = [ot.ClaytonCopulaFactory(),
             ot.FrankCopulaFactory(),
             ot.GumbelCopulaFactory(),
             ot.NormalCopulaFactory(),
             ot.StudentCopulaFactory()]
analysis.setDistributionsFactories(variables, factories)
myStudy.add(analysis)
print(analysis)

analysis.run()

result = analysis.getResult()
inferenceSet = result.getCopulaInferenceSetResult(variables)
print(inferenceSet.getBICResults())

# script
script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
