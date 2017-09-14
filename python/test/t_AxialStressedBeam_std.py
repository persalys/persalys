#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
dist_R = ot.LogNormalMuSigma(300., 30.).getDistribution()
dist_F = ot.Normal(75000., 5000.)

R = otguibase.Input('R', 300., dist_R, 'Yield stress (Pa)')
F = otguibase.Input('F', 75000., dist_F, 'Traction force (N)')
G = otguibase.Output('G', 'Stress difference (Pa)')

code = 'from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return G\n'
model = otguibase.PythonPhysicalModel('myPhysicalModel', [R, F], [G], code)
myStudy.add(model)

f = model.getFunction()
print(f([300.,75000.]))

## Design of Experiment - Parametric analysis ##
aDesign = otguibase.GridDesignOfExperiment('design', model)
aDesign.setLevels([2, 2])
myStudy.add(aDesign)
aDesign.run()

## Taylor Expansion ##
taylorExpansionsMoments = otguibase.TaylorExpansionMomentsAnalysis('myTaylorExpansionMoments', model)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

## Monte Carlo ##
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', model)
montecarlo.setMaximumCalls(1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

## Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', model)
sobol.setMaximumCoefficientOfVariation(-1)
sobol.setMaximumCalls(1000)
sobol.setBlockSize(250)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

## SRC ##
src = otguibase.SRCAnalysis('mySRC', model, 1000)
myStudy.add(src)
src.run()
srcResult = src.getResult()

## script
script = myStudy.getPythonScript()
print(script)
exec(script)
