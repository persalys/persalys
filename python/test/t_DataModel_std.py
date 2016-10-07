#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase
import os

myStudy = otguibase.OTStudy('myStudy')

filename = 'data.csv'
ot.RandomGenerator_SetSeed(0)
ot.Normal(3).getSample(10).exportToCSVFile(filename, ',')
columns = [0, 2]

myDataModel = otguibase.DataModel('myDataModel', "data.csv", columns)
myStudy.add(myDataModel)

myDataModelAnalysis = otguibase.DataAnalysis("myDataModelAnalysis", myDataModel)
myDataModelAnalysis.run()
myStudy.add(myDataModelAnalysis)

dataAnalysisResult = myDataModelAnalysis.getResult()
print(dataAnalysisResult.getMean())

# Comparaison
openturns.testing.assert_almost_equal(0.2012538261144671, dataAnalysisResult.getMean()[0], 1e-13)
openturns.testing.assert_almost_equal(-0.14315074899830527, dataAnalysisResult.getMean()[1], 1e-13)

script = myStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)