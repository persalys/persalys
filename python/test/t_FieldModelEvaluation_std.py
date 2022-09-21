#!/usr/bin/env python

import openturns as ot
import persalys

Study_0 = persalys.Study("Study_0")
persalys.Study.Add(Study_0)

# variables
z0 = persalys.Input("z0", 100)
v0 = persalys.Input("v0", 55)
m = persalys.Input("m", 80)
c = persalys.Input("c", 16)
z1 = persalys.Output("z1", "")
z2 = persalys.Output("z2", "fake output")
inputs = [z0, v0, m, c]
outputs = [z1, z2]

# mesh model
meshModel = persalys.GridMeshModel(ot.Interval(0.0, 12.0), [20])

# Python model
code = "from math import exp\n\ndef _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n    # mesh nodes\n    t = getMesh().getVertices()\n\n    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n    z2 = [2*max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n\n    return z, z2"
PhysicalModel_1 = persalys.PythonFieldModel(
    "PhysicalModel_1", meshModel, inputs, outputs, code
)
Study_0.add(PhysicalModel_1)

# evaluation
analysis1 = persalys.FieldModelEvaluation("analysis1", PhysicalModel_1)
Study_0.add(analysis1)
print(analysis1)

analysis1.run()
result1 = analysis1.getProcessSample()
print("result=", result1)

analysis2 = persalys.FieldModelEvaluation(
    "analysis2", PhysicalModel_1, [100, 55, 81, 17]
)
analysis2.setInterestVariables(["z2"])
Study_0.add(analysis2)
print(analysis2)

analysis2.run()
result2 = analysis2.getProcessSample()
print("result=", result2)

# script
script = Study_0.getPythonScript()
print(script)
exec(script)
