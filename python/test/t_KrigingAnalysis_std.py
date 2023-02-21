#!/usr/bin/env python

import openturns as ot
import openturns.testing
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

formula_y00 = "xi1"
formula_y0 = "cos(0.5*xi1) + sin(xi2)"
model = persalys.SymbolicPhysicalModel(
    "model", [xi1, xi2, xi3], [y00, y0], [formula_y00, formula_y0]
)
myStudy.add(model)

# Design of Experiment ##
aDesign = persalys.FixedDesignOfExperiment("design", model)
validationInputSample = ot.LHSExperiment(model.getDistribution(), 10).generate()
inputSample = ot.Sample(validationInputSample)
inputSample.stack(ot.Sample(10, [0.5]))
aDesign.setOriginalInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

# Kriging ##
analysis = persalys.KrigingAnalysis("kriging_0", aDesign)
analysis.setBasis(ot.LinearBasisFactory(2).build())
analysis.setCovarianceModel(ot.MaternModel(2))
myStudy.add(analysis)
print(analysis)

analysis.run()
metaModel = analysis.getResult().getResultForVariable("y0").getMetaModel()
openturns.testing.assert_almost_equal(
    aDesign.getResult().getDesignOfExperiment().getOutputSample(),
    metaModel(validationInputSample),
    3.0e-5,
    3.0e-5,
)

# Design of Experiment ##
model.addOutput(persalys.Output("y1"))
model.setFormula("y1", formula_y0 + " + xi3")
aDesign.setInterestVariables(["y0", "y1"])
aDesign.run()

# Kriging ##
analysis2 = persalys.KrigingAnalysis("kriging_1", aDesign)
analysis2.setAnalyticalValidation(True)
analysis2.setInterestVariables(["y1", "y0"])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

result2 = analysis2.getResult()
metaModel2 = result2.getResultForVariable("y1").getMetaModel()
openturns.testing.assert_almost_equal(
    aDesign.getResult().getDesignOfExperiment().getOutputSample().getMarginal(1),
    metaModel2(validationInputSample),
    3.0e-5,
    3.0e-5,
)
openturns.testing.assert_almost_equal(
    result2.getAnalyticalValidation().getQ2(), [0.685791] * 2, 1e-3, 1e-3
)

# Kriging ##
analysis3 = persalys.KrigingAnalysis("kriging_2", aDesign)
analysis3.setOptimizeParameters(False)
analysis3.setInterestVariables(["y0"])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
# print("result=", result3)

metaModel3 = result3.getResultForVariable("y0").getMetaModel()
openturns.testing.assert_almost_equal(
    aDesign.getResult().getDesignOfExperiment().getOutputSample().getMarginal(0),
    metaModel3(validationInputSample),
    3.0e-5,
    3.0e-5,
)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
