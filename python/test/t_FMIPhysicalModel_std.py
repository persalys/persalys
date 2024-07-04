#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import persalys
import os
import platform

myStudy = persalys.Study("myStudy")

key_platform = (platform.system(), platform.architecture()[0])
# Call to either 'platform.system' or 'platform.architecture' *after*
# importing pyfmi causes a segfault.
dict_platform = {("Linux", "64bit"): "linux64", ("Windows", "64bit"): "win64"}

path_here = os.path.dirname(os.path.abspath(__file__))
try:
    directory_platform = dict_platform[key_platform]
except KeyError:
    raise RuntimeError("Not available")

# CS model
path_fmu = os.path.join(path_here, "fmu", directory_platform, "deviation.fmu")

E = persalys.Input("E", 1.0, ot.Normal(), "E description")
F = persalys.Input("F", 1.0)
II = persalys.Input("I", 1.0)
L = persalys.Input("L", 1.0)
y = persalys.Output("y", "deviation")

inputs = [E, F, II, L]
outputs = [y]
model = persalys.FMIPhysicalModel("myPhysicalModel", inputs, outputs, path_fmu)
myStudy.add(model)

E_d = ot.Beta(0.93, 3.2 - 0.93, 28000000.0, 48000000.0)
F_d = ot.LogNormalMuSigma(30000.0, 9000.0, 15000.0).getDistribution()
L_d = ot.Uniform(250.0, 260.0)
I_d = ot.Beta(2.5, 4.0 - 2.5, 310.0, 450.0)
distribution = ot.JointDistribution([E_d, F_d, I_d, L_d])
x = distribution.getMean()

f = model.getFunction()
y = f(x)
ott.assert_almost_equal(y, [12.3369])

# ME model

path_fmu = os.path.join(path_here, "fmu", directory_platform, "epid.fmu")

healing_rate = persalys.Input("healing_rate", 0.02)
infection_rate = persalys.Input("infection_rate", 0.007)
infected = persalys.Output("infected")

inputs = [healing_rate, infection_rate]
outputs = [infected]
model2 = persalys.FMIPhysicalModel("myPhysicalModel2", inputs, outputs, path_fmu)
myStudy.add(model2)
x = [0.01, 0.004]

f = model2.getFunction()
y = f(x)
ott.assert_almost_equal(y, [434.668])

# script
script = myStudy.getPythonScript()
print("script=", script)
exec(script)
