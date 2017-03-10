#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
import os

anOTStudy = otguibase.OTStudy('anOTStudy')

## Model
X0 = otguibase.Input('X0', 1)
X1 = otguibase.Input('X1', 2)
Y0 = otguibase.Output('Y0')

model = otguibase.AnalyticalPhysicalModel('aModelPhys', [X0, X1], [Y0], ['sin(X0)+8*X1'])
anOTStudy.add(model)

## Design of Experiment ##
lowerBounds = [0.9, 1.8]
upperBounds = [1.1, 2.2]
levels = [2, 2]
aDesign = otguibase.FixedDesignOfExperiment('aDesign_1', model, lowerBounds, upperBounds, levels)
anOTStudy.add(aDesign)

aDesign.run()
print('outs=', aDesign.getOutputSample())

## Design of Experiment ##
filename = 'normal.csv'
ot.Normal(3).getSample(10).exportToCSVFile(filename)
aDesign2 = otguibase.FromFileDesignOfExperiment('aDesign_2', model, filename, [0, 2])
anOTStudy.add(aDesign2)

aDesign2.run()
print('outs=', aDesign2.getOutputSample())

## script
script = anOTStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
