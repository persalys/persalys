#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
import os

anOTStudy = otguibase.OTStudy('anOTStudy')

# Model
X0 = otguibase.Input('X0', 1, ot.Normal())
X1 = otguibase.Input('X1', 2)
Y0 = otguibase.Output('Y0')

model = otguibase.SymbolicPhysicalModel(
    'aModelPhys', [X0, X1], [Y0], ['sin(X0)+8*X1'])
anOTStudy.add(model)

# Design of Experiment ##
lowerBounds = [0.9, 1.8]
upperBounds = [1.1, 2.2]
levels = [2, 2]
aDesign = otguibase.GridDesignOfExperiment(
    'aDesign_1', model, lowerBounds, upperBounds, levels)
anOTStudy.add(aDesign)

aDesign.run()
print('outs=', aDesign.getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
filename = 'normal.csv'
ot.Normal(3).getSample(10).exportToCSVFile(filename)
aDesign2 = otguibase.ImportedDesignOfExperiment(
    'aDesign_2', model, filename, [0, 2])
anOTStudy.add(aDesign2)

aDesign2.run()
print('outs=', aDesign2.getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
aDesign3 = otguibase.ProbabilisticDesignOfExperiment(
    'aDesign_3', model, 10, 'QUASI_MONTE_CARLO')
anOTStudy.add(aDesign3)

aDesign3.run()
print('outs=', aDesign3.getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
aDesign4 = otguibase.FixedDesignOfExperiment('aDesign_4', model)
inputSample = ot.LHSExperiment(model.getDistribution(), 10).generate()
inputSample.stack(ot.Sample(10, [0.5]))
aDesign4.setOriginalInputSample(inputSample)
anOTStudy.add(aDesign4)

aDesign4.run()

print('outs=', aDesign4.getDesignOfExperiment().getOutputSample())

# script
script = anOTStudy.getPythonScript()
print(script)
exec(script)

os.remove(filename)
