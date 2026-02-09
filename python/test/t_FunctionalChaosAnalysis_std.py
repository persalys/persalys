#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import persalys

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

aDoe = myStudy.addDoEAsDataSet(aDesign)

# Chaos 1 ##
analysis = persalys.FunctionalChaosAnalysis("chaos_0", aDoe)
analysis.setChaosDegree(4)
analysis.setSparseChaos(True)
myStudy.add(analysis)
print(analysis)

analysis.run()

chaosResult = analysis.getResult()
sobolResult = chaosResult.getSobolResult()
# print("result=", chaosResult)
# print("functionalChaosResult", chaosResult.getFunctionalChaosResult())

# Comparaison
mean_ref = [-0.0114672, 0.488533]
variance_ref = [0.872994] * 2
fo_ref = [[0.52165, 0.47835]] * 2
to_ref = [[0.52165, 0.47835]] * 2

ott.assert_almost_equal(chaosResult.getMean(), mean_ref)
ott.assert_almost_equal(chaosResult.getVariance(), variance_ref)
ott.assert_almost_equal(sobolResult.getFirstOrderIndices(), fo_ref, 1e-3, 1e-3)
ott.assert_almost_equal(sobolResult.getTotalIndices(), to_ref, 1e-3, 1e-3)

# Chaos 2 ##
R = ot.CorrelationMatrix(2, [1, 0.5, 0.5, 1])
model.setCopula(["xi1", "xi2"], ot.NormalCopula(R))

design2 = persalys.ProbabilisticDesignOfExperiment("design2", model, 200, "MONTE_CARLO")
myStudy.add(design2)
design2.run()

doe2 = myStudy.addDoEAsDataSet(design2)

analysis2 = persalys.FunctionalChaosAnalysis("chaos_1", doe2)
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

ott.assert_almost_equal(chaosResult2.getAnalyticalValidation().getQ2(), [0.609145])
ott.assert_almost_equal(chaosResult2.getTestSampleValidation().getQ2(), [0.595006])
ott.assert_almost_equal(chaosResult2.getKFoldValidation().getQ2(), [0.601799])

# extract metamodel
metamodel = chaosResult2.getMetaModel()
ott.assert_almost_equal(metamodel.getFunction().getInputDimension(), 2)
ott.assert_almost_equal(metamodel.getFunction().getOutputDimension(), 1)
ott.assert_almost_equal(metamodel.getFunction()([0.5] * 2), [2.23523])
ott.assert_almost_equal(metamodel.getInputByName("xi1").getDistribution(), ot.Uniform(0., 10.))
assert model.getCopula() == ot.BlockIndependentCopula([ot.NormalCopula(ot.CorrelationMatrix(2, [1, 0.5, 0.5, 1]))])

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
