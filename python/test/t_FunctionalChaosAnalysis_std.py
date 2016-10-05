#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase
from math import *

ot.RandomGenerator.SetSeed(0)

myStudy = otguibase.OTStudy('myStudy')

## Model
xi1 = otguibase.Input('xi1', 0., '', ot.Uniform(-pi, pi))
xi2 = otguibase.Input('xi2', 0., '', ot.Uniform(-pi, pi))
xi3 = otguibase.Input('xi3', 0., '', ot.Uniform(-pi, pi))

# Create the Ishigami function
formula = "sin(xi1) + (7.0) * (sin(xi2)) ^ 2 + (0.1) * xi3^4 * sin(xi1)"
y0 = otguibase.Output('y0', 0., '', formula)

myPhysicalModel = otguibase.AnalyticalPhysicalModel('myPhysicalModel')

myPhysicalModel.addInput(xi1)
myPhysicalModel.addInput(xi2)
myPhysicalModel.addInput(xi3)

myPhysicalModel.addOutput(y0)

myStudy.add(myPhysicalModel)

## Design of Experiment ##
aDesign = otguibase.DesignOfExperiment('aDesign', myPhysicalModel)
inputSample = ot.LHSExperiment(myPhysicalModel.getComposedDistribution(), 250).generate()
aDesign.setInputSample(inputSample)
aDesign.run()
myStudy.add(aDesign)

## Chaos ##
chaos_0 = otguibase.FunctionalChaosAnalysis('chaos_0', aDesign)
chaos_0.setChaosDegree(6)
chaos_0.setSparseChaos(False)
chaos_0.setLeaveOneOutValidation(False)
myStudy.add(chaos_0)
chaos_0.run()
chaosResult = chaos_0.getResult()
sobolResult = chaosResult.getSobolResult()

# Comparaison
mean = [3.48612447253624591]
variance = [14.2028751935812352]
firstOrderIndices = [[0.30217019407129514, 0.441166953741984724, 0.00453456789773473614]]
totalIndices = [[0.548240000062856669, 0.462518011687460207, 0.248945824697117013]]

openturns.testing.assert_almost_equal(mean, chaosResult.getMean(), 1e-16)
openturns.testing.assert_almost_equal(variance, chaosResult.getVariance(), 1e-16)
openturns.testing.assert_almost_equal(firstOrderIndices, sobolResult.getFirstOrderIndices(), 1e-16)
openturns.testing.assert_almost_equal(totalIndices, sobolResult.getTotalIndices(), 1e-16)

script = myStudy.getPythonScript()
print(script)
exec(script)
