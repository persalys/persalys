#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import persalys

Study_0 = persalys.Study('Study_0')
persalys.Study.Add(Study_0)

# variables
dist_z0 = ot.Uniform(100, 150)
z0 = persalys.Input('z0', 100, dist_z0, '')
dist_v0 = ot.Normal(55, 10)
v0 = persalys.Input('v0', 55, dist_v0, '')
dist_m = ot.Normal(80, 8)
m = persalys.Input('m', 80, dist_m, '')
dist_c = ot.Uniform(0, 30)
c = persalys.Input('c', 16, dist_c, '')
z1 = persalys.Output('z1', '')
z2 = persalys.Output('z2', 'fake output')
inputs = [z0, v0, m, c]
outputs = [z1, z2]

# mesh model
meshModel = persalys.GridMeshModel(ot.Interval(0., 12.), [20])

# Python model
code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    z2 = [2*max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n\n    return z, z2'
PhysicalModel_1 = persalys.PythonFieldModel('PhysicalModel_1', meshModel, inputs, outputs, code)
Study_0.add(PhysicalModel_1)

# central tendency
mcAnalysis1 = persalys.FieldMonteCarloAnalysis("mcAnalysis", PhysicalModel_1)
mcAnalysis1.setMaximumCalls(10)
mcAnalysis1.setMaximumElapsedTime(1000)
mcAnalysis1.setBlockSize(5)
mcAnalysis1.setSeed(2)
mcAnalysis1.setInterestVariables(['z'])
mcAnalysis1.setKarhunenLoeveThreshold(2e-5)
Study_0.add(mcAnalysis1)

# evaluation
evaluation1 = persalys.FieldModelEvaluation("evaluation", PhysicalModel_1)
evaluation1.setInterestVariables(['z'])
Study_0.add(evaluation1)



