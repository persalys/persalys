#!/usr/bin/env python

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
f = model.getFunction()
print(f([[300.0, 75000.0]]))
print(f([[300.0, 75000.0], [400.0, 74000.0]]))

# test operator() (sample)
code = "from math import pi\nimport time\ndef _exec(R, F):\n"
code += (
    "    if R == 300.:\n      time.sleep(0.5)\n    G = 2*R-F/(pi*100.0)\n    return G\n"
)
model.setCode(code)
model.setProcessNumber(1)
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
f = model.getFunction()
try:
    print(f([[300.0, 75000.0]]))
except Exception as e:
    print(
        "ZeroDivisionError occurred: %s"
        % ("ZeroDivisionError: float division by zero" in str(e))
    )
    print("Error on the line 4 : %s" % ("line 4" in str(e)))

# script
script = myStudy.getPythonScript()
print(script)
exec(script)

# model definition clash bug
x1 = persalys.Input("x1")
x2 = persalys.Input("x2")
code1 = "def _exec(x1, x2):\n    y = x1 + x2\n    return y\n"
code2 = "def _exec(x1):\n    y = x1\n    return y\n"
y = persalys.Output("y")
model1 = persalys.PythonPhysicalModel("model1", [x1, x2], [y], code1)
model2 = persalys.PythonPhysicalModel("model1", [x1, x2], [y], code2)
f1 = model1.getFunction()
y1 = f1([1.0, 2.0])
f2 = model2.getFunction()
y2 = f2([1.0])
y1 = f1([2.0, 3.0])
