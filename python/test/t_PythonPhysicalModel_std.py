#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('OTStudy_0')
otguibase.OTStudy.Add(myStudy)

R = otguibase.Input('R', 0., 'Yield strength', ot.LogNormal(300., 30., 0., ot.LogNormal.MUSIGMA))
F = otguibase.Input('F', 0., 'Traction load', ot.Normal(75000., 5000.))
G = otguibase.Output('G', 0., 'deviation')

physicalModel_0 = otguibase.PythonPhysicalModel('physicalModel_0')
physicalModel_0.addInput(R)
physicalModel_0.addInput(F)
physicalModel_0.addOutput(G)
physicalModel_0.setCode('from math import pi\n\ndef _exec(X):\n    R = X[0]\n    F = X[1]\n    G = R-F/(pi*100.0)\n    return [G]\n')
myStudy.add(physicalModel_0)

f = physicalModel_0.getFunction()
print(f([300.,75000.]))

script = myStudy.getPythonScript()
print(script)
exec(script)
print('OK')