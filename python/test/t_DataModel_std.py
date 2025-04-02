#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os
import locale

myStudy = persalys.Study("myStudy")

# Model 1
filename = "data.csv"
ot.RandomGenerator.SetSeed(0)
ot.Normal(3).getSample(10).exportToCSVFile(filename)
inColumns = [0, 2]

model = persalys.DataModel("myDataModel", filename, inColumns)
myStudy.add(model)
print(model)

print("inputNames=", model.getInputNames())
print("outputNames=", model.getOutputNames())

# Data analysis ##
analysis = persalys.DataAnalysis("aDataAnalysis", model)
myStudy.add(analysis)

analysis.run()

result = analysis.getResult()
print("result=", result)
print("PDF=", result.getPDF())
print("CDF=", result.getCDF())
print("SurvivalFunction=", result.getSurvivalFunction())
print("outliers=", result.getOutliers())

# Comparaison
openturns.testing.assert_almost_equal(0.2012538261144671, result.getMean()[0][0], 1e-13)
openturns.testing.assert_almost_equal(
    -0.14315074899830527, result.getMean()[1][0], 1e-13
)

# Model 2
outColumns = [1]

model2 = persalys.DataModel(
    "myDataModel2", filename, inColumns, outColumns, ["var1", "var2"], ["var3"]
)
myStudy.add(model2)
print(model2)

print("inputNames=", model2.getInputNames())
print("outputNames=", model2.getOutputNames())

print("min=", model2.getListXMin())
print("max=", model2.getListXMax())

# Model 3
model3 = persalys.DataModel("myDataModel3", filename, inColumns, outColumns)
myStudy.add(model3)
print(model3)

print("inputNames=", model3.getInputNames())
print("outputNames=", model3.getOutputNames())

print("inputSample=", model3.getInputSample())
print("outputSample=", model3.getOutputSample())

print("min=", model3.getListXMin())
print("max=", model3.getListXMax())

# Quantile analysis
# Model 4
filename = "data_500.csv"
ot.RandomGenerator.SetSeed(0)
ot.Normal(3).getSample(500).exportToCSVFile(filename)
inColumns = [0, 1, 2]

model4 = persalys.DataModel("myDataModel4", filename, inColumns)
myStudy.add(model4)
print(model4)
print("inputNames=", model4.getInputNames())
print("outputNames=", model4.getOutputNames())

# Monte Carlo
analysis2 = persalys.QuantileAnalysis("aQuantileAnalysis", model4)
analysis2.setTargetProbabilities([[1e-2, 1e-3], [6e-3], [1e-1, 1e-2, 1e-3]])
analysis2.setTailTypes(
    [persalys.QuantileAnalysisResult.Upper | persalys.QuantileAnalysisResult.Lower | persalys.QuantileAnalysisResult.Bilateral,
     persalys.QuantileAnalysisResult.Lower,
     persalys.QuantileAnalysisResult.Bilateral])

analysis2.setType(persalys.QuantileAnalysisResult.MonteCarlo)
myStudy.add(analysis2)
analysis2.run()
result = analysis2.getResult()

x0ref = [[[-2.46389, -2.23369, -2.0035],
          [-2.51804, -2.4067, -2.29536]],
         [[1.92083, 2.30459, 2.68835],
          [2.85639, 3.15958, 3.46277]],
         [[-2.47455, -2.29006, -2.10558],
          [-2.48545, -2.4067, -2.32795]],
         [[2.41487, 3.02799, 3.64111],
          [2.94514, 3.15958, 3.37402]]]

x1ref = [[[-3.36709, -2.84159, -2.31609]]]

x2ref = [[[-1.73368, -1.5705, -1.40732],
          [-3.6264, -2.98535, -2.3443],
          [-3.32293, -3.09834, -2.87375]],
         [[1.51902, 1.69843, 1.87784],
          [2.29461, 2.5038, 2.71299],
          [2.53902, 2.63821, 2.7374]]]

for i, qx in enumerate(result.getQuantiles('X0')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x0ref[i][j])
for i, qx in enumerate(result.getQuantiles('X1')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x1ref[i][j])
for i, qx in enumerate(result.getQuantiles('X2')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x2ref[i][j])

print(result.getWilksValidity('X0', persalys.QuantileAnalysisResult.Upper))
print(result.getWilksValidity('X0', persalys.QuantileAnalysisResult.Lower))
print(result.getWilksValidity('X0', persalys.QuantileAnalysisResult.Bilateral))
print(result.getWilksValidity('X1', persalys.QuantileAnalysisResult.Lower))
print(result.getWilksValidity('X2', persalys.QuantileAnalysisResult.Bilateral))

# Generalized Pareto
# test wrong threshold
analysis2.setType(persalys.QuantileAnalysisResult.GeneralizedPareto)
try:
    analysis2.run()
except Exception as e:
    print(e)
analysis2.setTargetProbabilities([[1e-2, 1e-3], [6e-3], [2e-2, 1e-2, 1e-3]])
analysis2.run()

# correct threshold
ot.RandomGenerator.SetSeed(0)
analysis2.setThreshold(ot.Sample([[-1.2] * 3, [1.3] * 3]))
print(analysis2.getCDFThreshold())
analysis2.run()

lower = persalys.QuantileAnalysisResult.Lower
upper = persalys.QuantileAnalysisResult.Upper
result = analysis2.getResult()
x0ref = [[[-2.25066, -2.13742, -1.97106],
          [-2.63007, -2.42975, -2.00384]],
         [[2.0336, 2.39169, 2.66636],
          [2.10793, 3.24011, 3.91598]],
         [[-2.40182, -2.26008, -2.10424],
          [-2.72218, -2.47166, -2.13712]],
         [[2.29473, 2.68407, 3.05917],
          [2.35091, 3.43519, 4.37443]]]

x1ref = [[[-3.20317, -2.67809, -2.29305]]]

x2ref = [[[-2.5951, -2.33329, -2.05612],
          [-2.90863, -2.60824, -2.09622],
          [-4.16267, -3.37042, -2.1222]],
         [[2.1073, 2.30144, 2.43061],
          [2.14865, 2.43789, 2.55157],
          [2.17072, 2.67384, 2.85496]]]

for i, qx in enumerate(result.getQuantiles('X0')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x0ref[i][j])
for i, qx in enumerate(result.getQuantiles('X1')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x1ref[i][j])
for i, qx in enumerate(result.getQuantiles('X2')):
    for j, qxi in enumerate(qx):
        openturns.testing.assert_almost_equal(qxi, x2ref[i][j])

openturns.testing.assert_almost_equal(result.getPValue('X0', upper), 0.847489013, 1e-3)
openturns.testing.assert_almost_equal(result.getPValue('X0', lower), 0.633769532, 1e-3)
openturns.testing.assert_almost_equal(result.getPValue('X1', lower), 0.957051222, 1e-3)
openturns.testing.assert_almost_equal(result.getPValue('X2', upper), 0.409330454, 1e-3)
openturns.testing.assert_almost_equal(result.getPValue('X2', lower), 0.556529202, 1e-3)

# test interest variables
analysis2.setInterestVariables(["X0", "X2"])
analysis2.setTailTypes(
    [persalys.QuantileAnalysisResult.Upper | persalys.QuantileAnalysisResult.Lower | persalys.QuantileAnalysisResult.Bilateral,
     persalys.QuantileAnalysisResult.Bilateral])
analysis2.setTargetProbabilities([[1e-2, 1e-3], [2e-2, 1e-2, 1e-3]])
analysis2.setThreshold(ot.Sample([[-1.2] * 2, [1.3] * 2]))
print(analysis2.getCDFThreshold())
analysis2.run()

# script
script = myStudy.getPythonScript()
print(script)
exec(script)

# check ambiguous import
sample = ot.Normal(2).getSample(10)
sample[0] = [1, 2]
inColumns = [0, 1]
have_fr_locale = True
try:
    locale.setlocale(locale.LC_ALL, "fr_FR.utf8")
except Exception:
    have_fr_locale = False

if have_fr_locale:
    for col_sep in [";", ",", " "]:
        for num_sep in [".", ","]:
            if col_sep == num_sep:
                continue
            with open(filename, "w") as csv:
                csv.write('"x"' + col_sep + ' "y"\n')
                for p in sample:
                    for j in range(len(p)):
                        csv.write(str(p[j]).replace(".", num_sep))
                        if j < len(p) - 1:
                            csv.write(col_sep)
                    csv.write("\n")
            model5 = persalys.DataModel("myDataModel5", filename, inColumns)
            assert model5.getSampleFromFile().getDimension() == 2, (
                "wrong dimension sep=" + col_sep
            )
            assert model5.getSampleFromFile().getSize() == 10, "wrong size"
    os.remove(filename)

    # test latin1 chars
    filename = "DonneesLatin1.csv"
    model5 = persalys.DataModel("myDataModel5", filename, [0, 1, 2, 3], [4, 5])

    myStudy.add(model5)
    assert "Ann" in model5.getSampleFromFile().getDescription()[0]
