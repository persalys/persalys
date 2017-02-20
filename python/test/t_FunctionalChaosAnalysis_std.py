#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

ot.RandomGenerator.SetSeed(0)

myStudy = otguibase.OTStudy('myStudy')

## Model
xi1 = otguibase.Input('xi1', 0., '', ot.Uniform(0., 10.))
xi2 = otguibase.Input('xi2', 0., '', ot.Uniform(0., 10.))
y00 = otguibase.Output('fake_y0')
y00.setIsSelected(False)
y0 = otguibase.Output('y0')
y1 = otguibase.Output('y1')

formula_y00 = "xi1"
formula_y0 = "cos(0.5*xi1) + sin(xi2)"
formula_y1 = "cos(0.5*xi1) + sin(xi2) + 0.5"
model = otguibase.AnalyticalPhysicalModel('model', [xi1, xi2], [y00, y0, y1], [formula_y00, formula_y0, formula_y1])
myStudy.add(model)

## Design of Experiment ##
aDesign = otguibase.DesignOfExperimentImplementation('design', model)
inputSample = ot.LHSExperiment(model.getComposedDistribution(), 50).generate()
aDesign.setInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

## Chaos 1 ##
analysis = otguibase.FunctionalChaosAnalysis('chaos_0', aDesign)
analysis.setChaosDegree(4)
analysis.setSparseChaos(True)
myStudy.add(analysis)
print(analysis)

analysis.run()

chaosResult = analysis.getResult()
sobolResult = chaosResult.getSobolResult()
print("result=", chaosResult)
print("functionalChaosResult", chaosResult.getFunctionalChaosResult())

# Comparaison
mean = [0, 0.4885327735406139]
variance = [0.8727523923767868, 0.8729938152031875]
firstOrderIndices = [[ 0.5218735936795287, 0.4781264063204713 ], [ 0.5216496993795265, 0.4783503006204734 ]]
totalIndices = [[ 0.5218735936795287, 0.4781264063204713 ], [ 0.5216496993795265, 0.4783503006204734 ]]

openturns.testing.assert_almost_equal(mean, chaosResult.getMean(), 1e-16)
openturns.testing.assert_almost_equal(variance, chaosResult.getVariance(), 1e-16)
openturns.testing.assert_almost_equal(firstOrderIndices, sobolResult.getFirstOrderIndices(), 1e-16)
openturns.testing.assert_almost_equal(totalIndices, sobolResult.getTotalIndices(), 1e-16)

## Chaos 2 ##
analysis2 = otguibase.FunctionalChaosAnalysis('chaos_1', aDesign)
analysis2.setChaosDegree(4)
analysis2.setLeaveOneOutValidation(True)
analysis2.setInterestVariables(['y1'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

chaosResult2 = analysis2.getResult()
sobolResult2 = chaosResult2.getSobolResult()
print("result=", chaosResult2)
print("functionalChaosResult", chaosResult2.getFunctionalChaosResult())

openturns.testing.assert_almost_equal([0.9223441081335271], chaosResult2.getQ2LeaveOneOut(), 1e-16)

## script
script = myStudy.getPythonScript()
print(script)
exec(script)
