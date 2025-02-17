#!/usr/bin/env python

import openturns as ot
import persalys

myStudy = persalys.Study("myStudy")

# mesh points are on the first line
meshModel = persalys.GridMeshModel(
    [persalys.Variable("t", "date")], ot.Interval([1], [12]), [12])
print(meshModel.getMesh().getVertices())

model = persalys.DataFieldModel("myModel", meshModel)
model.importProcessSample("elNino.csv", persalys.Tools.Columns)

myStudy.add(model)
# check that the processSample can be properly converted (used for gui init)
print(model.getProcessSampleAsSample())

analysis1 = persalys.FieldKarhunenLoeveAnalysis("myAnalysis", model)
analysis1.run()
print(analysis1)

myStudy.add(analysis1)

result1 = analysis1.getResult()
print(result1)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
