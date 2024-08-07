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
            model = persalys.DataModel("myDataModel2", filename, inColumns)
            assert model.getSampleFromFile().getDimension() == 2, (
                "wrong dimension sep=" + col_sep
            )
            assert model.getSampleFromFile().getSize() == 10, "wrong size"
    os.remove(filename)

    # test latin1 chars
    filename = "DonneesLatin1.csv"
    model4 = persalys.DataModel("myDataModel4", filename, [0, 1, 2, 3], [4, 5])

    myStudy.add(model4)
    assert "Ann" in model4.getSampleFromFile().getDescription()[0]
