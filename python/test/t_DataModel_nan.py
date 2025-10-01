import persalys
import openturns.testing as ott
import openturns as ot
from math import nan
from sys import stderr

study = persalys.Study("DataModel_nan")

# Model 1
model_1 = persalys.DataModel("Model_1", "jdd_test_avec_NaN.txt", [0, 1], [2])
study.add(model_1)

analysis_1 = persalys.DataAnalysis("analysis_1", model_1)
study.add(analysis_1)
analysis_1.run()

result_1 = analysis_1.getResult()
ott.assert_almost_equal(result_1.getMean()[0][0], 209712)
ott.assert_almost_equal(result_1.getMean()[2][0], 161441)
ott.assert_almost_equal(result_1.getEffectiveSize(), ot.Point([20, 9, 26]))

# Model 2
model_2 = persalys.DataModel("Model_2", "jdd_test_no_multivar.txt", [0, 1], [2])
study.add(model_2)

analysis_2 = persalys.DataAnalysis("analysis_2", model_2)
study.add(analysis_2)
analysis_2.run()

result_2 = analysis_2.getResult()
ott.assert_almost_equal(result_2.getMultivariateDoE().getSample().getSize(), 0)

# Model 3
model_3 = persalys.DataModel("Model_3", "jdd_test_empty_marginal.txt", [0, 1], [2])
study.add(model_3)

analysis_3 = persalys.DataAnalysis("analysis_3", model_3)
study.add(analysis_3)
analysis_3.run()

result_3 = analysis_3.getResult()
ott.assert_almost_equal(result_3.getDesignOfExperiment().getSample().getDimension(), 2)

# Model 3 modification
model_3.setInputSample(ot.Sample([[0, 1], [1, 0], [1, nan]]))
model_3.setOutputSample(ot.Sample([[1], [1], [nan]]))

analysis_3_2 = persalys.DataAnalysis("analysis_3_2", model_3)
study.add(analysis_3_2)
analysis_3_2.run()

result_3_2 = analysis_3_2.getResult()
ott.assert_almost_equal(result_3_2.getEffectiveSize(), ot.Point([3, 2, 2]))

# Model 4
print("model 4", file=stderr)
model_4 = persalys.DataModel("Model_4", "jdd_test_avec_NaN.txt", [0, 1, 2])
study.add(model_4)

analysis_4 = persalys.DataAnalysis("analysis_4", model_4)
study.add(analysis_4)
analysis_4.run()
