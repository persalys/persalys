import openturns as ot
import openturns.testing as ott
import persalys
from math import sin, exp

myStudy = persalys.Study("myStudy")

# +++++++++++++++++++++++++++++++++ Model 1 +++++++++++++++++++++++++++++++++ #
# simple example with a data model
importedDataset1 = persalys.ImportedDataset(
    "Housing-prices-Boston.csv", list(range(13)), [13]
)
model1 = persalys.DataModel("model1", importedDataset1)
myStudy.add(model1)

# Sensitivity analysis
analysis1 = persalys.DataSensitivityAnalysis("analysis1", model1)
myStudy.add(analysis1)
analysis1.run()

result1 = analysis1.getResult()
ott.assert_almost_equal(
    result1.getFirstOrderSobolIndices()[0],
    [
        0.269789,
        0.712661,
        0.709397,
        0.687175,
        0.698848,
        0.626429,
        0.166526,
        0.240896,
        0.69844,
        0.71514,
        0.707562,
        0.267871,
        0.649162,
    ],
)
ott.assert_almost_equal(
    result1.getFirstOrderSobolIndicesInterval()[0].getLowerBound(),
    [
        0.190449,
        0.0968657,
        0.325595,
        -0.104384,
        0.497543,
        0.541836,
        0.0273068,
        0.123202,
        0.0962543,
        0.243149,
        0.268534,
        0.0559757,
        0.600002,
    ],
)
ott.assert_almost_equal(
    result1.getFirstOrderSobolIndicesInterval()[0].getUpperBound(),
    [
        0.363531,
        0.307685,
        0.547926,
        0.142314,
        0.647108,
        0.688973,
        0.218533,
        0.33305,
        0.314734,
        0.460371,
        0.482617,
        0.225703,
        0.699414,
    ],
)

ott.assert_almost_equal(
    result1.getSignedSRCIndices(),
    [
        [
            -0.100175,
            0.117651,
            0.0155602,
            0.0742493,
            -0.224215,
            0.290666,
            0.00229872,
            -0.33788,
            0.289376,
            -0.225936,
            -0.22444,
            0.0932346,
            -0.407997,
        ]
    ],
)
ott.assert_almost_equal(
    result1.getSRCIndices(), [[x**2 for x in result1.getSignedSRCIndices()[0]]]
)

ott.assert_almost_equal(
    result1.getSignedSRCIndicesInterval()[0].getLowerBound()[:3],
    [-0.151228, 0.0624121, -0.0406055],
)
ott.assert_almost_equal(
    result1.getSignedSRCIndicesInterval()[0].getUpperBound()[:3],
    [-0.0307025, 0.171362, 0.0924589],
)

ott.assert_almost_equal(result1.getR2()[0], 0.638872)

assert (
    result1.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global).getSize() == 0
)
assert (
    result1.getR2HSICIndices(persalys.DataSensitivityAnalysisResult.Global).getSize()
    == 0
)
assert (
    result1.getPValuesAsymptotic(
        persalys.DataSensitivityAnalysisResult.Global
    ).getSize()
    == 0
)
assert (
    result1.getPValuesPermutation(
        persalys.DataSensitivityAnalysisResult.Global
    ).getSize()
    == 0
)


# +++++++++++++++++++++++++++++++++ Model 2 +++++++++++++++++++++++++++++++++ #
# Model with several outputs and dependences between inputs
ot.RandomGenerator.SetSeed(0)

# ------------------------------model definition----------------------------- #
marginals = [
    ot.Normal(),  # X0
    ot.Uniform(0, 1),  # X1
    ot.Normal(),  # X2
    ot.Normal(),  # X3
    ot.Normal(),  # X4
]
R = ot.CorrelationMatrix(5)
R[0, 2] = 0.3
R[3, 4] = -0.2
copula = ot.NormalCopula(R)
distribution = ot.JointDistribution(marginals, copula)
distribution.setDescription(["X0", "X1", "X2", "X3", "X4"])


def model2_def(X):
    x0, x1, x2, x3, x4 = X
    y0 = x0 + x1**2 + sin(x2)
    y1 = x3 * x4 + exp(-x0)
    y2 = x1 + x2 + x3 + x4
    return [y0, y1, y2]


ot_model2 = ot.PythonFunction(5, 3, model2_def)

sample_size = 300
sample = distribution.getSample(sample_size)
output_sample = ot_model2(sample)
output_sample.setDescription(["Y0", "Y1", "Y2"])
sample.stack(output_sample)


# saving the sample to a CSV file
filename = "DataSensitivityAnalysis_std.csv"
sample.exportToCSVFile(filename)

# ------------------------test of the persalys model------------------------- #
importedDataset2 = persalys.ImportedDataset(filename, list(range(5)), list(range(5, 8)))
model2 = persalys.DataModel("model2", importedDataset2)
myStudy.add(model2)

analysis2 = persalys.DataSensitivityAnalysis("analysis2", model2)
myStudy.add(analysis2)
analysis2.run()

result2 = analysis2.getResult()

ott.assert_almost_equal(
    result2.getFirstOrderSobolIndices(),
    [
        [0.731891, -0.0296164, 0.489005, 0.0508961, -0.054944],
        [0.695839, 0.122029, 0.0114455, 0.12667, 0.0458143],
        [0.0247123, 0.00144117, 0.467444, 0.192548, 0.204893],
    ],
)

ott.assert_almost_equal(
    result2.getFirstOrderSobolIndicesInterval()[0].getLowerBound(),
    [0.695092, -0.14747, 0.433013, -0.058401, -0.145523],
)

ott.assert_almost_equal(
    result2.getFirstOrderSobolIndicesInterval()[2].getUpperBound(),
    [0.141488, 0.110519, 0.517855, 0.291101, 0.28865],
)

assert not result2.isIndependent()
assert len(result2.getIndependenceWarningMessage()) > 0

# -------------------------test without output sample------------------------ #
importedDataset2_no_output = persalys.ImportedDataset(filename, list(range(5)), [])
model2_no_output = persalys.DataModel("model2_no_output", importedDataset2_no_output)
myStudy.add(model2_no_output)

analysis2_no_output = persalys.DataSensitivityAnalysis(
    "analysis2_no_output", model2_no_output
)
myStudy.add(analysis2_no_output)
with ott.assert_raises(Exception):
    analysis2_no_output.run()


# +++++++++++++++++++++++++++++++++ Model 3 +++++++++++++++++++++++++++++++++ #
# Test the constructor with type parameter and HSIC analysis
ot.RandomGenerator.SetSeed(0)

# ------------------------------ model definition --------------------------- #
sample3 = ot.Normal(3).getSample(100)
output_sample3 = ot.Sample([[x[0] + x[1] ** 2 + x[2]] for x in sample3])
output_sample3.setDescription(["Y0"])
sample3.stack(output_sample3)
filename3 = "DataSensitivityAnalysis_type_std.csv"
sample3.exportToCSVFile(filename3)

importedDataset3 = persalys.ImportedDataset(filename3, [0, 1, 2], [3])
model3 = persalys.DataModel("model3", importedDataset3)
myStudy.add(model3)

RankSobol = persalys.DataSensitivityAnalysisResult.RankSobol
SRC = persalys.DataSensitivityAnalysisResult.SRC
GlobalHSIC = persalys.DataSensitivityAnalysisResult.GlobalHSIC

# --------------------- Test with type = RankSobol only --------------------- #
analysis3_sobol = persalys.DataSensitivityAnalysis("analysis3_sobol", model3, RankSobol)
myStudy.add(analysis3_sobol)
analysis3_sobol.run()
result3_sobol = analysis3_sobol.getResult()

ott.assert_almost_equal(
    result3_sobol.getFirstOrderSobolIndices()[0],
    [0.109269, 0.4685, 0.185635],
)
# SRC and HSIC should be empty
assert (
    result3_sobol.getSRCIndices().getSize() == 0
), "SRC should be empty when only RankSobol is requested"
assert (
    len(result3_sobol.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global))
    == 0
), "HSIC should be empty when only RankSobol is requested"

# ----------------------- Test with type = SRC only ------------------------- #
analysis3_src = persalys.DataSensitivityAnalysis("analysis3_src", model3, SRC)
myStudy.add(analysis3_src)
analysis3_src.run()
result3_src = analysis3_src.getResult()

ott.assert_almost_equal(
    result3_src.getSRCIndices(),
    [[0.157031, 0.00808757, 0.243416]],
)
# Sobol and HSIC should be empty
assert (
    len(result3_src.getFirstOrderSobolIndices()) == 0
), "Sobol should be empty when only SRC is requested"
assert (
    len(result3_src.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global)) == 0
), "HSIC should be empty when only SRC is requested"

# --------------- Test with type = GlobalHSIC with V-statistic -------------- #
covModels3 = ot.CovarianceModelCollection([ot.SquaredExponential() for _ in range(4)])
analysis3_hsic_v = persalys.DataSensitivityAnalysis(
    "analysis3_hsic_v", model3, GlobalHSIC
)
analysis3_hsic_v.setCovarianceModels(
    covModels3, persalys.DataSensitivityAnalysisResult.Global
)
analysis3_hsic_v.setHSICParameters(
    False, True, False, persalys.DataSensitivityAnalysisResult.Global
)  # asymptotic only, V-stat
myStudy.add(analysis3_hsic_v)
analysis3_hsic_v.run()
result3_hsic_v = analysis3_hsic_v.getResult()

ott.assert_almost_equal(
    result3_hsic_v.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global)[0],
    [0.0145197, 0.00998482, 0.0207494],
)
ott.assert_almost_equal(
    result3_hsic_v.getR2HSICIndices(persalys.DataSensitivityAnalysisResult.Global)[0],
    [0.230779, 0.148265, 0.306366],
)
ott.assert_almost_equal(
    result3_hsic_v.getPValuesAsymptotic(persalys.DataSensitivityAnalysisResult.Global)[
        0
    ],
    [7.96808e-08, 2.2285e-05, 6.5083e-12],
    1e-3,
    1e-15,
)
# Sobol and SRC should be empty
assert (
    len(result3_hsic_v.getFirstOrderSobolIndices()) == 0
), "Sobol should be empty when only HSIC is requested"
assert (
    result3_hsic_v.getSRCIndices().getSize() == 0
), "SRC should be empty when only HSIC is requested"
assert (
    len(
        result3_hsic_v.getPValuesPermutation(
            persalys.DataSensitivityAnalysisResult.Global
        )
    )
    == 0
), "Permutation p-values should be empty when only asymptotic p-values are requested"

# --------------- Test with type = GlobalHSIC with U-statistic -------------- #
analysis3_hsic_u = persalys.DataSensitivityAnalysis(
    "analysis3_hsic_u", model3, GlobalHSIC
)
analysis3_hsic_u.setCovarianceModels(
    covModels3, persalys.DataSensitivityAnalysisResult.Global
)
analysis3_hsic_u.setHSICParameters(
    True, True, True, persalys.DataSensitivityAnalysisResult.Global
)  # asymptotic, permutation, U-stat
myStudy.add(analysis3_hsic_u)
analysis3_hsic_u.run()
result3_hsic_u = analysis3_hsic_u.getResult()

ott.assert_almost_equal(
    result3_hsic_u.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global)[0],
    [0.013004, 0.0080629, 0.0195008],
)
ott.assert_almost_equal(
    result3_hsic_u.getR2HSICIndices(persalys.DataSensitivityAnalysisResult.Global)[0],
    [0.209012, 0.120633, 0.290126],
)
ott.assert_almost_equal(
    result3_hsic_u.getPValuesAsymptotic(persalys.DataSensitivityAnalysisResult.Global)[
        0
    ],
    [7.23848e-08, 3.28072e-05, 3.73796e-12],
    1e-3,
    1e-15,
)

# ----------------------- Test with all types combined ---------------------- #
analysis3_all = persalys.DataSensitivityAnalysis(
    "analysis3_all", model3, RankSobol | SRC | GlobalHSIC
)
analysis3_all.setCovarianceModels(
    covModels3, persalys.DataSensitivityAnalysisResult.Global
)
analysis3_all.setHSICParameters(
    False, True, False, persalys.DataSensitivityAnalysisResult.Global
)  # asymptotic only, V-stat
myStudy.add(analysis3_all)
analysis3_all.run()
result3_all = analysis3_all.getResult()

# All result types should be populated
assert len(result3_all.getFirstOrderSobolIndices()) > 0, "Sobol should not be empty"
assert result3_all.getSRCIndices().getSize() > 0, "SRC should not be empty"
assert (
    len(result3_all.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global)) > 0
), "HSIC should not be empty"
ott.assert_almost_equal(
    result3_all.getHSICIndices(persalys.DataSensitivityAnalysisResult.Global)[0],
    [0.0145197, 0.00998482, 0.0207494],
)
