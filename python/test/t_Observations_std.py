#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os

ot.RandomGenerator.SetSeed(0)

anOTStudy = persalys.Study("anOTStudy")

# Model
X0 = persalys.Input("X0", 1, ot.Normal())
X1 = persalys.Input("X1", 2)
X2 = persalys.Input("X2", 2)
X3 = persalys.Input("X3", 2)
Y0 = persalys.Output("Y0")
Y1 = persalys.Output("Y1")
Y2 = persalys.Output("Y2")

model = persalys.SymbolicPhysicalModel(
    "aModelPhys", [X0, X1, X2, X3], [Y0, Y1, Y2], ["sin(X0)+8*X1"] * 3
)
anOTStudy.add(model)

# Observations ##
filename = "normal2.csv"
sample = ot.Normal(8).getSample(10)
sample.exportToCSVFile(filename)

obs = persalys.Observations(
    "obs", model, filename, [7, 3], [6, 2], ["X3", "X1"], ["Y2", "Y1"]
)
anOTStudy.add(obs)
print("obs=", obs)

openturns.testing.assert_almost_equal(
    obs.getSample(), sample.getMarginal([3, 7, 2, 6]), 1e-16
)

# - reload file
sample2 = sample * 3
sample2.exportToCSVFile(filename)
obs.setFileName(filename)
openturns.testing.assert_almost_equal(
    obs.getSample(), sample2.getMarginal([3, 7, 2, 6]), 1e-16
)

# - change columns
obs.setColumns([4, 5], ["X3", "X1"], [3, 0], ["Y2", "Y1"])
openturns.testing.assert_almost_equal(
    obs.getSample(), sample2.getMarginal([5, 4, 0, 3]), 1e-16
)

try:
    obs.setColumns([4, 5, 0], ["X3", "X1"], [3, 0], ["Y2", "Y1"])
except Exception as e:
    print(
        "InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e))
    )

try:
    obs.setColumns([4, 5, 0], ["X0", "X1", "X2"], [], [])
except Exception as e:
    print(
        "InvalidArgumentException occured: %s"
        % (
            "InvalidArgumentException : Define observations for at least an output"
            in str(e)
        )
    )

obs.setColumns([4, 5, 1], ["X0", "X1", "X2"], [3, 0], ["Y1", "Y2"])
assert obs.getSample().getDescription() == ["X0", "X1", "X2", "Y1", "Y2"]

# - change observed variables
obs.setColumns([4, 5, 1], ["X3", "X2", "X1"], [3, 0], ["Y1", "Y0"])
assert obs.getSample().getDescription() == ["X1", "X2", "X3", "Y0", "Y1"]

# Observations ##
inS = sample.getMarginal([7, 3])
inS.setDescription(["X3", "X1"])
outS = sample.getMarginal([6, 2])
outS.setDescription(["Y2", "Y1"])
obs2 = persalys.Observations("obs2", model, inS, outS)
anOTStudy.add(obs2)
print("obs2=", obs2)

openturns.testing.assert_almost_equal(
    obs2.getSample(), sample.getMarginal([3, 7, 2, 6]), 1e-16
)

# script
script = anOTStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
