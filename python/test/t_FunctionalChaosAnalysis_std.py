#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase
from math import pi

ot.RandomGenerator.SetSeed(0)

myStudy = otguibase.OTStudy('myStudy')

## Model
xi1 = otguibase.Input('xi1', 0., '', ot.Uniform(-pi, pi))
xi2 = otguibase.Input('xi2', 0., '', ot.Uniform(-pi, pi))
xi3 = otguibase.Input('xi3', 0., '', ot.Uniform(-pi, pi))
y00 = otguibase.Output('fake_y0', 0, '', 'xi1')
y00.setIsSelected(False)
formula = "sin(xi1) + (7.0) * (sin(xi2)) ^ 2 + (0.1) * xi3^4 * sin(xi1)"
y0 = otguibase.Output('y0', 0., '', formula)

model = otguibase.AnalyticalPhysicalModel('model', [xi1, xi2, xi3], [y00, y0])
myStudy.add(model)

## Design of Experiment ##
aDesign = otguibase.DesignOfExperiment('design', model)
inputSample = ot.LHSExperiment(model.getComposedDistribution(), 250).generate()
aDesign.setInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

## Chaos ##
analysis = otguibase.FunctionalChaosAnalysis('chaos_0', aDesign)
analysis.setChaosDegree(6)
analysis.setSparseChaos(True)
myStudy.add(analysis)
print(analysis)

analysis.run()

chaosResult = analysis.getResult()
sobolResult = chaosResult.getSobolResult()
print("result=", chaosResult)
print("functionalChaosResult", chaosResult.getFunctionalChaosResult())

# Comparaison
mean = [3.52577795807805572]
variance = [13.4272570693236712]
firstOrderIndices = [[0.316310314256740444,0.452262731654470784,0]]
totalIndices = [[0.54773726834552916,0.452262731654470784,0.231426954088788689]]

openturns.testing.assert_almost_equal(mean, chaosResult.getMean(), 1e-16)
openturns.testing.assert_almost_equal(variance, chaosResult.getVariance(), 1e-16)
openturns.testing.assert_almost_equal(firstOrderIndices, sobolResult.getFirstOrderIndices(), 1e-16)
openturns.testing.assert_almost_equal(totalIndices, sobolResult.getTotalIndices(), 1e-16)


## Design of Experiment ##
model.addOutput(otguibase.Output('y1', 0., '', formula))
aDesign2 = otguibase.DesignOfExperiment('design2', model)
inputSample = ot.LHSExperiment(model.getComposedDistribution(), 150).generate()
aDesign2.setInputSample(inputSample)
myStudy.add(aDesign2)

aDesign2.run()

## Chaos ##
analysis2 = otguibase.FunctionalChaosAnalysis('chaos_1', aDesign2)
analysis2.setChaosDegree(6)
analysis2.setLeaveOneOutValidation(True)
analysis2.setOutputsToAnalyse(['y1'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

chaosResult2 = analysis2.getResult()
sobolResult2 = chaosResult2.getSobolResult()
print("result=", chaosResult2)
print("functionalChaosResult", chaosResult2.getFunctionalChaosResult())

## script
script = myStudy.getPythonScript()
print(script)
exec(script)
