#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import persalys


pyscript = 'def _exec(x, y, p):\n    w, z = 3*x+4*y+p,x+y+p\n    return w, z\n'

model = persalys.YACSPhysicalModel('myPhysicalModel', persalys.InputCollection(), persalys.OutputCollection(), pyscript)

inputSample = [[1, 2, 4], [2, 3, 5], [6, 1, 3]]

evalSample = [[15, 7], [23, 10], [25, 10]]

resultSample = model.getFunction()(inputSample)
print(resultSample)

# Comparison
ott.assert_almost_equal(evalSample, resultSample, 1e-16)


# script
myStudy = persalys.Study('myStudy')
myStudy.add(model)
script = myStudy.getPythonScript()
#print('script=', script)
exec(script)
