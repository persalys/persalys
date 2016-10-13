#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
R = otguibase.Input('R', 0., 'Yield strength', ot.LogNormalMuSigma(300., 30.).getDistribution())
F = otguibase.Input('F', 0., 'Traction load', ot.Normal(75000., 5000.))
G = otguibase.Output('G', 0., 'deviation')
code = 'from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return [G]\n'

model = otguibase.PythonPhysicalModel('myPhysicalModel', [R, F], [G], code)
myStudy.add(model)

f = model.getFunction()
print(f([300.,75000.]))

model.setCode('from math import pi\n\ndef _exec(R, F):\n    G = 2*R-F/(pi*100.0)\n    return [G]\n')
f = model.getFunction()
print(f([300.,75000.]))

## script
script = myStudy.getPythonScript()
print(script)
exec(script)