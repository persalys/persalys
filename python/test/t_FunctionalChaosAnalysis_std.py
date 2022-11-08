#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import persalys

ot.RandomGenerator.SetSeed(0)

myStudy = persalys.Study("myStudy")

# Model
xi1 = persalys.Input("xi1", ot.Uniform(0.0, 10.0))
xi2 = persalys.Input("xi2", ot.Uniform(0.0, 10.0))
xi3 = persalys.Input("xi3", 0.5)
y00 = persalys.Output("fake_y0")
y00.setIsSelected(False)
y0 = persalys.Output("y0")
y1 = persalys.Output("y1")

formula_y00 = "xi1"
formula_y0 = "cos(0.5*xi1) + sin(xi2)"
formula_y1 = "cos(0.5*xi1) + sin(xi2) + xi3"
model = persalys.SymbolicPhysicalModel(
    "model", [xi1, xi2, xi3], [y00, y0, y1], [formula_y00, formula_y0, formula_y1]
)
myStudy.add(model)

# Design of Experiment ##
aDesign = persalys.FixedDesignOfExperiment("design", model)
inputSample = ot.LHSExperiment(model.getDistribution(), 50).generate()
inputSample.stack(ot.Sample(50, [0.5]))
aDesign.setOriginalInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

# Chaos 1 ##
analysis = persalys.FunctionalChaosAnalysis("chaos_0", aDesign)
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
mean = [0, 0.4883827512409983]
variance = [0.8595249944148248, 0.8597699643961747]
firstOrderIndices = [
    [0.511719012189697, 0.4882809878103031],
    [0.5114868181064884, 0.48851318189351145],
]
totalIndices = [
    [0.511719012189697, 0.4882809878103031],
    [0.5114868181064884, 0.48851318189351145],
]

ott.assert_almost_equal(mean, chaosResult.getMean(), 1e-16)
ott.assert_almost_equal(variance, chaosResult.getVariance(), 1e-16)
ott.assert_almost_equal(firstOrderIndices, sobolResult.getFirstOrderIndices(), 1e-16)
ott.assert_almost_equal(totalIndices, sobolResult.getTotalIndices(), 1e-16)

# Chaos 2 ##
analysis2 = persalys.FunctionalChaosAnalysis("chaos_1", aDesign)
analysis2.setChaosDegree(4)
analysis2.setAnalyticalValidation(True)
analysis2.setTestSampleValidation(True)
analysis2.setTestSampleValidationPercentageOfPoints(30)
analysis2.setTestSampleValidationSeed(2)
analysis2.setKFoldValidation(True)
analysis2.setKFoldValidationNumberOfFolds(4)
analysis2.setKFoldValidationSeed(2)
analysis2.setInterestVariables(["y1"])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

chaosResult2 = analysis2.getResult()
sobolResult2 = chaosResult2.getSobolResult()
print("result=", chaosResult2)
print("functionalChaosResult", chaosResult2.getFunctionalChaosResult())

ott.assert_almost_equal([0.840009], chaosResult2.getAnalyticalValidation().getQ2())
ott.assert_almost_equal(
    [0.9457431125210048], chaosResult2.getTestSampleValidation().getQ2(), 1e-16
)
ott.assert_almost_equal(
    [0.9143738630455855], chaosResult2.getKFoldValidation().getQ2(), 1e-16
)

# extract metamodel
metamodel = chaosResult2.getMetaModel()
ott.assert_almost_equal(metamodel.getFunction().getInputDimension(), 2)
ott.assert_almost_equal(metamodel.getFunction().getOutputDimension(), 1)
ott.assert_almost_equal(metamodel.getFunction()([0.5] * 2), [2.17011])


# script
script = myStudy.getPythonScript()
print(script)
exec(script)
