#!/usr/bin/env python

import multiprocessing

import openturns as ot
import openturns.testing
import persalys

myStudy = persalys.Study("myStudy")

# Model
R = persalys.Input(
    "R", 0.0, ot.LogNormalMuSigma(300.0, 30.0).getDistribution(), "Yield strength"
)
F = persalys.Input("F", 0.0, ot.Normal(75000.0, 5000.0), "Traction load")
G = persalys.Output("G", "deviation")
code = "from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return G\n"

model = persalys.PythonPhysicalModel("myPhysicalModel", [R, F], [G], code)
myStudy.add(model)

f = model.getFunction()
print(f([300.0, 75000.0]))

# several outputs
model.setCode(
    "from math import pi\n\ndef _exec(R, F):\n    G =  6*F\n    H = 42.0\n    return G, H\n"
)
f = model.getFunction()
print(f([300.0, 75000.0]))

# test operator() (sample)
model.setCode(
    "from math import pi\n\ndef _exec(R, F):\n    G = 2*R-F/(pi*100.0)\n    return G\n"
)
model.setParallel(True)
model.setProcessNumber(2)
f = model.getFunction()
print(f([[300.0, 75000.0]]))
print(f([[300.0, 75000.0], [400.0, 74000.0]]))

# test operator() (sample)
code = "from math import pi\nimport time\ndef _exec(R, F):\n"
code += (
    "    if R == 300.:\n      time.sleep(0.5)\n    G = 2*R-F/(pi*100.0)\n    return G\n"
)
model.setCode(code)
plan_0 = persalys.FixedDesignOfExperiment(
    "plan_0", model, [[300.0, 75000.0], [400.0, 74000.0]]
)
plan_0.setBlockSize(2)
myStudy.add(plan_0)
plan_0.run()
print("Sample :\n %s" % plan_0.getResult().getDesignOfExperiment().getSample())

# model with an error
model.setCode(
    "from math import pi\n\ndef _exec(R, F):\n    G = 2*R-F/(pi*100.0)/0.\n    return G\n"
)
model.setParallel(True)
f = model.getFunction()
try:
    print(f([[300.0, 75000.0]]))
except Exception as e:
    ok = "ZeroDivisionError" in str(e)
    print(f"ZeroDivisionError occurred: {ok}")
    print("Error on the line 4 : %s" % ("line 4" in str(e)))

# the workers must be started once and reused by the following batches: restarting them
# for every batch used to cost far more than the evaluation itself, which made a block
# size greater than 1 much slower than a block size of 1 and prevented analyses from
# converging in time
model.setCode(
    "from math import pi\n\ndef _exec(R, F):\n    G = 2*R-F/(pi*100.0)\n    return G\n"
)
model.setParallel(True)
model.setProcessNumber(2)
f = model.getFunction()

f([[300.0, 75000.0], [400.0, 74000.0], [500.0, 73000.0]])
print(
    "workers still running after the batch: %s"
    % (len(multiprocessing.active_children()) > 0)
)

# and reused batches must keep giving the same values
for i in range(3):
    inputSample = [[300.0 + i, 75000.0 - i], [400.0 + i, 74000.0 - i]]
    print(f(inputSample))

# Sobol' with a block size greater than 1 must converge and must give the same result as
# a block size of 1
sobolStudy = persalys.Study("sobolStudy")
sobolStudy.add(model)


def runSobol(name, blockSize):
    analysis = persalys.SobolAnalysis(name, model)
    # converge on the first iteration, so that the result does not depend on the machine
    analysis.setMaximumConfidenceIntervalLength(1.0)
    analysis.setMaximumElapsedTime(60)
    analysis.setReplicationSize(16)
    analysis.setBlockSize(blockSize)
    analysis.setSeed(0)
    sobolStudy.add(analysis)
    analysis.run()
    result = analysis.getResult()
    return (
        result.getCallsNumber(),
        [round(value, 6) for value in result.getFirstOrderIndices()[0]],
        round(result.getConfidenceIntervalLength(), 6),
    )


reference = runSobol("sobol_1", 1)
print("callsNumber = %d" % reference[0])
print("firstOrderIndices = %s" % reference[1])
print("confidenceIntervalLength = %s" % reference[2])
for blockSize in (2, 8):
    same = runSobol("sobol_%d" % blockSize, blockSize) == reference
    print("blockSize=%d gives the same result: %s" % (blockSize, same))

# script
model.setCode(code)
script = myStudy.getPythonScript()
print(script)
exec(script)
