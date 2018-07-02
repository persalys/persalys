#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import otguibase

Study_0 = otguibase.Study('Study_0')
otguibase.Study.Add(Study_0)

# variables
z0 = otguibase.Input('z0', 100, '')
v0 = otguibase.Input('v0', 55, '')
m = otguibase.Input('m', 80, '')
c = otguibase.Input('c', 15, '')
z = otguibase.Output('z', '')
z2 = otguibase.Output('z2', '')
inputs = [z0, v0, m, c]
outputs = [z]

# mesh model
meshModel = otguibase.GridMeshModel(ot.Interval(0., 12.), [20])

# Symbolic model
formula = ["max(0, z0 + (m * 9.81 / c) * t + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-t * c / m)))"]
model1 = otguibase.SymbolicFieldModel('model1', meshModel, inputs, outputs, formula)
Study_0.add(model1)

f = model1.getPointToFieldFunction()
print(f([100, 55, 81, 17]))
print(f([[100, 55, 81, 17], [100, 55, 81, 18]]))


# Symbolic model : two outputs
outputs = [z, z2]
formula = ["max(0, z0 + (m * 9.81 / c) * t + (m / c) * (v0 - (m * 9.81 / c)) * (1 - exp(-t * c / m)))"] * 2
model2 = otguibase.SymbolicFieldModel('model2', meshModel, inputs, outputs, formula)
Study_0.add(model2)

f = model2.getPointToFieldFunction()
print(f([100, 55, 81, 17]))
print(f([[100, 55, 81, 17], [100, 55, 81, 18]]))


# script
script = Study_0.getPythonScript()
print(script)
exec(script)