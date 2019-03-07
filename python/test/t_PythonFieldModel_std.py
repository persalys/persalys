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
inputs = [z0, v0, m, c]
outputs = [z]

# mesh model
meshModel = otguibase.GridMeshModel(ot.Interval(0., 12.), [20])

# Python model
code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    return z'
model = otguibase.PythonFieldModel('model', meshModel, inputs, outputs, code)
Study_0.add(model)

f = model.getPointToFieldFunction()
print(f([100, 55, 81, 17]))
print(f([[100, 55, 81, 17], [100, 55, 81, 18]]))

# model with an error : ZeroDivisionError
model.setCode(
    'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81 / 0.\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = mesh.getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    return z')
model.setParallel(True)
f = model.getPointToFieldFunction()
try:
  print(f([[100, 55, 81, 17]]))
except Exception as e:
  print("ZeroDivisionError occured: %s" % ("ZeroDivisionError: float division by zero" in str(e)))
  print("Error on the line 4 : %s" % ("line 4" in str(e)))
try:
  print(f([100, 55, 81, 17]))
except Exception as e:
  print("ZeroDivisionError occured: %s" % ("ZeroDivisionError: float division by zero" in str(e)))
  print("Error on the line 4 : %s" % ("line 4" in str(e)))

# model with an error : wrong output type
model.setCode('def _exec(z0,v0,m,c):\n    z = z0\n    return z')

f = model.getPointToFieldFunction()

try:
  print(f([100, 55, 81, 17]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  print(f([[100, 55, 81, 17]]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))

# model with an error : wrong output sequence size
model.setCode('def _exec(z0,v0,m,c):\n    z = [z0]*2\n    return z')

f = model.getPointToFieldFunction()

try:
  print(f([100, 55, 81, 17]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  print(f([[100, 55, 81, 17]]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))

# Python model with two outputs
z1 = otguibase.Output('z1', '')
outputs = [z, z1]

code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    z1 = z[:]\n    return z, z1'
model2 = otguibase.PythonFieldModel('model2', meshModel, inputs, outputs, code)
model2.setParallel(True)
Study_0.add(model2)

f = model2.getPointToFieldFunction()

print(f([100, 55, 81, 17]))
print(f([[100, 55, 81, 17], [100, 55, 81, 18]]))


# model2 with an error : wrong output sequence size
code = 'from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    z1 = [2.]\n    return z, z1'
model.setCode(code)

f = model2.getPointToFieldFunction()

try:
  print(f([100, 55, 81, 17]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  print(f([[100, 55, 81, 17]]))
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))


# script
script = Study_0.getPythonScript()
print(script)
exec(script)
