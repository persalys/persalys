import openturns as ot
import openturns.testing as ott
import persalys
from math import sin, exp

myStudy = persalys.Study("myStudy")

# +++++++++++++++++++++++++++++++++ Model 1 +++++++++++++++++++++++++++++++++ #
# simple example with a data model
model1 = persalys.DataModel(
    "model1", "Housing-prices-Boston.csv", list(range(13)), [13]
)
myStudy.add(model1)
print(model1)
print("inputNames=", model1.getInputNames())
print("outputNames=", model1.getOutputNames())

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

# +++++++++++++++++++++++++++++++++ Model 2 +++++++++++++++++++++++++++++++++ #
# Model with several outputs and dependences between inputs

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
ot.RandomGenerator.SetSeed(0)
sample = distribution.getSample(sample_size)
output_sample = ot_model2(sample)
output_sample.setDescription(["Y0", "Y1", "Y2"])
sample.stack(output_sample)


# saving the sample to a CSV file
filename = "DataSensitivityAnalysis_std.csv"
sample.exportToCSVFile(filename)

# ------------------------test of the persalys model------------------------- #
model2 = persalys.DataModel("model2", filename, list(range(5)), list(range(5, 8)))
myStudy.add(model2)
print()
print(model2)

analysis2 = persalys.DataSensitivityAnalysis("analysis2", model2)
myStudy.add(analysis2)
analysis2.run()

result2 = analysis2.getResult()
for i in range(3):
    print(f"Output Y{i} first order indices: ", result2.getFirstOrderSobolIndices()[i])
    print(
        f"Output Y{i} first order indices intervals: ",
        result2.getFirstOrderSobolIndicesInterval()[i],
    )

print("Is independent ?", result2.isIndependent())
print("Warning message: ", result2.getIndependenceWarningMessage())

# -------------------------test without output sample------------------------ #
model2_no_output = persalys.DataModel("model2_no_output", filename, list(range(5)), [])
myStudy.add(model2_no_output)

analysis2_no_output = persalys.DataSensitivityAnalysis(
    "analysis2_no_output", model2_no_output
)
myStudy.add(analysis2_no_output)
with ott.assert_raises(Exception):
    analysis2_no_output.run()
