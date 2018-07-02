#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import otguibase

Study_0 = otguibase.Study('Study_0')
otguibase.Study.Add(Study_0)

# variables
dist_z0 = ot.Uniform(100, 150)
z0 = otguibase.Input('z0', 100, dist_z0, '')
dist_v0 = ot.Normal(55, 10)
v0 = otguibase.Input('v0', 55, dist_v0, '')
dist_m = ot.Normal(80, 8)
m = otguibase.Input('m', 80, dist_m, '')
dist_c = ot.Uniform(0, 30)
c = otguibase.Input('c', 16, dist_c, '')
z1 = otguibase.Output('z1', '')
z2 = otguibase.Output('z2', 'fake output')
inputs = [z0, v0, m, c]
outputs = [z1, z2]

# mesh model
meshModel = otguibase.GridMeshModel(ot.Interval(0., 12.), [20])

# Python model
code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    z2 = [2*max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n\n    return z, z2'
PhysicalModel_1 = otguibase.PythonFieldModel('PhysicalModel_1', meshModel, inputs, outputs, code)
Study_0.add(PhysicalModel_1)

# central tendency
mcAnalysis1 = otguibase.FieldMonteCarloAnalysis("mcAnalysis1", PhysicalModel_1)
mcAnalysis1.setMaximumCalls(30)
Study_0.add(mcAnalysis1)
print(mcAnalysis1)

mcAnalysis1.run()
result1 = mcAnalysis1.getResult()
print("result=", result1)
print("eigen values=", result1.getKarhunenLoeveResults()[0].getEigenValues(), result1.getKarhunenLoeveResults()[1].getEigenValues())

PhysicalModel_1.setParallel(True)
c.setStochastic(False)
mcAnalysis2 = otguibase.FieldMonteCarloAnalysis("mcAnalysis2", PhysicalModel_1)
mcAnalysis2.setMaximumCalls(100)
mcAnalysis2.setMaximumElapsedTime(50)
mcAnalysis2.setBlockSize(10)
mcAnalysis2.setInterestVariables(['z2'])
Study_0.add(mcAnalysis2)
print(mcAnalysis2)

mcAnalysis2.run()
result2 = mcAnalysis2.getResult()
print("eigen values=", result2.getKarhunenLoeveResults()[0].getEigenValues())

# script
script = Study_0.getPythonScript()
print(script)
exec(script)
