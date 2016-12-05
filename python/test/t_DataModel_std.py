#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase
import os

myStudy = otguibase.OTStudy('myStudy')

## Model 1
filename = 'data.csv'
ot.RandomGenerator_SetSeed(0)
ot.Normal(3).getSample(10).exportToCSVFile(filename)
inColumns = [0, 2]

model = otguibase.DataModel('myDataModel', filename, inColumns)
myStudy.add(model)
print(model)

print("inputNames=", model.getInputNames())
print("outputNames=", model.getOutputNames())

## Data analysis ##
analysis = otguibase.DataAnalysis("aDataAnalysis", model)
myStudy.add(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print("PDF=", result.getPDF())
print("CDF=", result.getCDF())
print("outliers=", result.getOutliers())

# Comparaison
openturns.testing.assert_almost_equal(0.2012538261144671, result.getMean()[0][0], 1e-13)
openturns.testing.assert_almost_equal(-0.14315074899830527, result.getMean()[1][0], 1e-13)

## Model 2
outColumns = [1]

model2 = otguibase.DataModel('myDataModel2', filename, inColumns, outColumns, ["var1", "var2"], ["var3"])
myStudy.add(model2)
print(model2)

print("inputNames=", model2.getInputNames())
print("outputNames=", model2.getOutputNames())

## Model 3
model3 = otguibase.DataModel('myDataModel3', filename, inColumns, outColumns)
myStudy.add(model3)
print(model3)

print("inputNames=", model3.getInputNames())
print("outputNames=", model3.getOutputNames())

print("inputSample=", model3.getInputSample())
print("outputSample=", model3.getOutputSample())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)