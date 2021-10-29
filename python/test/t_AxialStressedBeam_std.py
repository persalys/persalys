#!/usr/bin/env python

import openturns as ot
import openturns.testing
import persalys

myStudy = persalys.Study('myStudy')

# Model
dist_R = ot.LogNormalMuSigma(300., 30.).getDistribution()
dist_F = ot.Normal(75000., 5000.)

R = persalys.Input('R', 300., dist_R, 'Yield stress (Pa)')
F = persalys.Input('F', 75000., dist_F, 'Traction force (N)')
G = persalys.Output('G', 'Stress difference (Pa)')

code = 'from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return G\n'
model = persalys.PythonPhysicalModel('myPhysicalModel', [R, F], [G], code)
myStudy.add(model)

f = model.getFunction()
print(f([300., 75000.]))

# Design of Experiment - Parametric analysis ##
bounds = persalys.GridDesignOfExperiment.GetDefaultBounds(model)
values = [[bounds.getLowerBound()[i], bounds.getUpperBound()[i]] for i in range(2)]
aDesign = persalys.GridDesignOfExperiment('design', model, values)
myStudy.add(aDesign)
aDesign.run()

# Taylor Expansion ##
taylorExpansionsMoments = persalys.TaylorExpansionMomentsAnalysis(
    'myTaylorExpansionMoments', model)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

# Monte Carlo ##
montecarlo = persalys.MonteCarloAnalysis('myMonteCarlo', model)
montecarlo.setMaximumCalls(1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

# Sobol ##
sobol = persalys.SobolAnalysis('mySobol', model)
sobol.setMaximumConfidenceIntervalLength(-1)
sobol.setMaximumCalls(1000)
sobol.setReplicationSize(250)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

# SRC ##
src = persalys.SRCAnalysis('mySRC', model, 1000)
myStudy.add(src)
src.run()
srcResult = src.getResult()

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
