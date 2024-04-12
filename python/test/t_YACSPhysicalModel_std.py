#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import persalys


pyscript = "def _exec(x, y, p):\n    w, z = 3*x+4*y+p,x+y+p\n    return w, z\n"

model = persalys.YACSPhysicalModel("myPhysicalModel", [], [], pyscript)

X = [[1, 2, 4], [2, 3, 5], [6, 1, 3]]

Y_ref = [[15, 7], [23, 10], [25, 10]]

# full eval
f = model.getFunction()
Y = f(X)
print(Y)
ott.assert_almost_equal(Y, Y_ref)

# partial eval
f = model.getFunction(["w"])
Y = f(X)
print(Y)
ott.assert_almost_equal(Y, ot.Sample(Y_ref).getMarginal(0))

# script
myStudy = persalys.Study("myStudy")
myStudy.add(model)
script = myStudy.getPythonScript()
# print('script=', script)
exec(script)

# test for output variables order
pyscript = "def _exec(inp1):\n    aaa,bbb,ddd,ccc = 1.,2.,3.,inp1\n    return aaa,bbb,ddd,ccc\n"
model = persalys.YACSPhysicalModel("order", [], [], pyscript)
x = [8.0]
y_ref = [1.0, 2.0, 3.0, 8.0]
f = model.getFunction()
y = f(x)
# ott.assert_almost_equal(y, y_ref)

pyscript = "def _exec(x1, x2):\n    y1, y2 = 1/x1, x2\n    return y1, y2\n"
model = persalys.YACSPhysicalModel("myPhysicalModelError", [], [], pyscript)
model.setParallel(True)
model.setProcessNumber(4)

X = [[0, 1], [1, 1]]

design = persalys.FixedDesignOfExperiment("myDesign", model, X)
design.run()
print("outs=", design.getErrorDescription())

Y_ref = [[1, 1]]
ott.assert_almost_equal(
    design.getResult().getDesignOfExperiment().getOutputSample(), Y_ref
)
