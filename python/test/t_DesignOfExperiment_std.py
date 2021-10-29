#! /usr/bin/env python

import openturns as ot
import persalys
import os

anOTStudy = persalys.Study('anOTStudy')

# Model
X0 = persalys.Input('X0', 1, ot.Normal())
X1 = persalys.Input('X1', 2, ot.Normal())
Y0 = persalys.Output('Y0')

model = persalys.SymbolicPhysicalModel(
    'aModelPhys', [X0, X1], [Y0], ['sin(X0)+8*X1'])
anOTStudy.add(model)

# Design of Experiment ##
aDesign = persalys.GridDesignOfExperiment('aDesign_1', model, [[0.9, 1.1], [1.8, 2.2]])
anOTStudy.add(aDesign)

aDesign.run()
print('outs=', aDesign.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
filename = 'normal.csv'
ot.Normal(3).getSample(10).exportToCSVFile(filename)
aDesign2 = persalys.ImportedDesignOfExperiment(
    'aDesign_2', model, filename, [0, 2])
anOTStudy.add(aDesign2)

aDesign2.run()
print('outs=', aDesign2.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
aDesign3 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_3', model, 10, 'QUASI_MONTE_CARLO')
anOTStudy.add(aDesign3)

aDesign3.run()
print('outs=', aDesign3.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment ##
aDesign4 = persalys.FixedDesignOfExperiment('aDesign_4', model)
inputSample = ot.LHSExperiment(model.getDistribution(), 10).generate()
#inputSample.stack(ot.Sample(10, [0.5, 1.3]))
aDesign4.setOriginalInputSample(inputSample)
anOTStudy.add(aDesign4)

aDesign4.run()

print('outs=', aDesign4.getResult().getDesignOfExperiment().getOutputSample())

# 3D Model to test Space filling algos
X0 = persalys.Input('X0', 1, ot.Normal())
X1 = persalys.Input('X1', 2, ot.Normal())
X2 = persalys.Input('X2', 3, ot.Normal())
Y0 = persalys.Output('Y0')

model = persalys.SymbolicPhysicalModel(
    'aModelPhys', [X0, X1, X2], [Y0], ['sin(X0)+8*X1*X2'])
anOTStudy2 = persalys.Study('anOTStudy2')
anOTStudy2.add(model)

# Design of Experiment SALHS-PhiP#
aDesign5 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_5', model, 10, 'SALHS', 'PhiP')
anOTStudy2.add(aDesign5)

aDesign5.run()
print('outs=', aDesign5.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment SALHS-minDist#
aDesign6 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_6', model, 10, 'SALHS', 'minDist')
aDesign6.run()

print('outs=', aDesign6.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment SALHS-C2#
aDesign7 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_7', model, 10, 'SALHS', 'C2', 1000)
anOTStudy2.add(aDesign7)

aDesign7.run()
print('outs=', aDesign7.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment MCLHS-PhiP#
aDesign8 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_8', model, 10, 'MCLHS', 'PhiP', 1000)
anOTStudy2.add(aDesign8)

aDesign8.run()
print('outs=', aDesign8.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment MCLHS-minDist#
aDesign9 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_9', model, 10, 'MCLHS', 'minDist', 1000)
anOTStudy2.add(aDesign9)

aDesign9.run()
print('outs=', aDesign9.getResult().getDesignOfExperiment().getOutputSample())

# Design of Experiment MCLHS-C2#
aDesign10 = persalys.ProbabilisticDesignOfExperiment(
    'aDesign_10', model, 10, 'MCLHS', 'C2', 1000)
anOTStudy2.add(aDesign10)

aDesign10.run()
print('outs=', aDesign10.getResult().getDesignOfExperiment().getOutputSample())

# script
script = anOTStudy.getPythonScript()
print(script)
exec(script)

script = anOTStudy2.getPythonScript()
print(script)
exec(script)


os.remove(filename)
