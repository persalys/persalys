#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
import os

anOTStudy = otguibase.OTStudy('anOTStudy')
X0 = otguibase.Input('X0', 1, '')
X1 = otguibase.Input('X1', 2, '')
Y0 = otguibase.Output('Y0', 0, '', 'sin(X0)+8*X1')
aModelPhys = otguibase.AnalyticalPhysicalModel('aModelPhys')
aModelPhys.addInput(X0)
aModelPhys.addInput(X1)
aModelPhys.addOutput(Y0)
anOTStudy.add(aModelPhys)
lowerBounds = [0.9, 1.8]
upperBounds = [1.1, 2.2]
levels = [2, 2]
aDesign = otguibase.FixedDesignOfExperiment('aDesign', aModelPhys, lowerBounds, upperBounds, levels)
anOTStudy.add(aDesign)
aDesign.evaluate()
print('outs=', aDesign.getResult().getOutputSample())

filename = 'normal.csv'
ot.Normal(3).getSample(10).exportToCSVFile(filename, ',')
columns = [0, 2]
aDesign2 = otguibase.FromFileDesignOfExperiment('aDesign2', aModelPhys, filename, ot.Indices(columns))
anOTStudy.add(aDesign2)
aDesign2.evaluate()
print('outs=', aDesign2.getResult().getOutputSample())

script = anOTStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
