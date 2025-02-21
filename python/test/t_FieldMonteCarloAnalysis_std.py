#!/usr/bin/env python

import openturns as ot
import persalys

Study_0 = persalys.Study("Study_0")
persalys.Study.Add(Study_0)

# variables
dist_z0 = ot.Uniform(100, 150)
z0 = persalys.Input("z0", 100, dist_z0, "")
dist_v0 = ot.Normal(55, 10)
v0 = persalys.Input("v0", 55, dist_v0, "")
dist_m = ot.Normal(80, 8)
m = persalys.Input("m", 80, dist_m, "")
dist_c = ot.Uniform(0, 30)
c = persalys.Input("c", 16, dist_c, "")
z1 = persalys.Output("z1", "")
z2 = persalys.Output("z2", "fake output")
inputs = [z0, v0, m, c]
outputs = [z1, z2]

# mesh model
meshModel = persalys.GridMeshModel(ot.Interval(0.0, 12.0), [20])

# Python model
code = "from math import exp\n\n"
code += "def _exec(z0,v0,m,c):\n    g = 9.81\n    zmin = 0.\n    tau = m / c\n    vinf = -m * g / c\n\n"
code += "    # mesh nodes\n    t = getMesh().getVertices()\n\n"
code += "    z = [max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n"
code += "    z2 = [2*max(z0 + vinf * t_i[0] + tau * (v0 - vinf) * (1 - exp(-t_i[0] / tau)), zmin) for t_i in t]\n\n"
code += "    return z, z2"
PhysicalModel_1 = persalys.PythonFieldModel(
    "PhysicalModel_1", meshModel, inputs, outputs, code
)
Study_0.add(PhysicalModel_1)

# central tendency
mcAnalysis1 = persalys.FieldMonteCarloAnalysis("mcAnalysis1", PhysicalModel_1)
mcAnalysis1.setMaximumCalls(30)
Study_0.add(mcAnalysis1)
print(mcAnalysis1)

mcAnalysis1.run()
result1 = mcAnalysis1.getResult()
print("result=", result1)
print(
    "eigen values=",
    result1.getKarhunenLoeveResults()[0].getEigenvalues(),
    result1.getKarhunenLoeveResults()[1].getEigenvalues(),
)

PhysicalModel_1.setParallel(True)
c.setStochastic(False)
mcAnalysis2 = persalys.FieldMonteCarloAnalysis("mcAnalysis2", PhysicalModel_1)
mcAnalysis2.setMaximumCalls(100)
mcAnalysis2.setMaximumElapsedTime(50)
mcAnalysis2.setBlockSize(10)
mcAnalysis2.setInterestVariables(["z2"])
Study_0.add(mcAnalysis2)
print(mcAnalysis2)

mcAnalysis2.run()
result2 = mcAnalysis2.getResult()
print("eigen values=", result2.getKarhunenLoeveResults()[0].getEigenvalues())

# export z evaluation in csv
processSample = mcAnalysis1.getResult().getProcessSample()

fieldSample = ot.Sample(0, meshModel.getMesh().getVertices().getSize())
for field in processSample:
    fieldSample.add(field.getMarginal(1).asPoint())
fieldSample.exportToCSVFile("zDoe.csv")

# import DoE in DataFieldModel
dataModel = persalys.DataFieldModel("dataModel", meshModel)
dataModel.importProcessSample("zDoe.csv", persalys.Tools.Columns)
Study_0.add(dataModel)

analysis3 = persalys.FieldKarhunenLoeveAnalysis("analysis3", dataModel)
analysis3.run()
result3 = analysis3.getResult()
print(result1.getXiSamples()[0] == result3.getXiSamples()[0])
Study_0.add(analysis3)

# script
script = Study_0.getPythonScript()
print(script)
exec(script)
