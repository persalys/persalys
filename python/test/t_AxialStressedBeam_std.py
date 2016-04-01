#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

R = otguibase.Input('R', 0., 'Yield strength', ot.LogNormal(300., 30., 0., ot.LogNormal.MUSIGMA))
F = otguibase.Input('F', 0., 'Traction load', ot.Normal(75000., 5000.))
G = otguibase.Output('G', 0., 'deviation')

myPhysicalModel = otguibase.PythonPhysicalModel('myPhysicalModel')
myPhysicalModel.addInput(R)
myPhysicalModel.addInput(F)
myPhysicalModel.addOutput(G)
myPhysicalModel.setCode('from math import pi\n\ndef _exec(R, F):\n    G = R-F/(pi*100.0)\n    return [G]\n')

f = myPhysicalModel.getFunction()
print(f([300.,75000.]))

myStudy.add(myPhysicalModel)

## Design of Experiment - Parametric analysis ##
aDesign = otguibase.DesignOfExperiment('aDesign', myPhysicalModel)
myStudy.add(aDesign)
aDesign.evaluate()

## Quadratic Cumul ##
taylorExpansionsMoments = otguibase.TaylorExpansionMomentsAnalysis('myTaylorExpansionMoments', myPhysicalModel)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

## Monte Carlo ##
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', myPhysicalModel, 1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

## Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', myPhysicalModel, 1000)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

## SRC ##
src = otguibase.SRCAnalysis('mySRC', myPhysicalModel, 1000)
myStudy.add(src)
src.run()
srcResult = src.getResult()


script = myStudy.getPythonScript()
print(script)
exec(script)
