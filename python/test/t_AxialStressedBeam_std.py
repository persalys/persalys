#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
R = otguibase.Input('R', 0., 'Yield strength', ot.LogNormalMuSigma(300., 30.).getDistribution())
F = otguibase.Input('F', 0., 'Traction load', ot.Normal(75000., 5000.))
G = otguibase.Output('G', 0., 'deviation')

model = otguibase.PythonPhysicalModel('myPhysicalModel')
model.addInput(R)
model.addInput(F)
model.addOutput(G)
model.setCode('from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return [G]\n')
myStudy.add(model)

f = model.getFunction()
print(f([300.,75000.]))

## Design of Experiment - Parametric analysis ##
aDesign = otguibase.DesignOfExperiment('design', model)
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
sobol.setBlockSize(1000)
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
