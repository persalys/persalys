#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

R = otguibase.Input('R', 0., 'Yield strength', ot.LogNormal(300., 30., 0., ot.LogNormal.MUSIGMA))
F = otguibase.Input('F', 0., 'Traction load', ot.Normal(75000., 5000.))

G = otguibase.Output('Ep', 0., 'deviation', 'R-F/(_pi*100.0)')

myPhysicalModel = otguibase.AnalyticalPhysicalModel('myPhysicalModel')

myPhysicalModel.addInput(R)
myPhysicalModel.addInput(F)

myPhysicalModel.addOutput(G)

myStudy.add(myPhysicalModel)

## Design of Experiment - Parametric analysis ##
aDesign = otguibase.DesignOfExperiment('aDesign', myPhysicalModel)
myStudy.add(aDesign)
aDesign.evaluate()

## Quadratic Cumul ##
taylorExpansionsMoments = otguibase.TaylorExpansionsMomentsAnalysis('myTaylorExpansionsMoments', myPhysicalModel)
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
sobol = otguibase.SobolAnalysis('mySobol', myPhysicalModel)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

## SRC ##
src = otguibase.SRCAnalysis('mySRC', myPhysicalModel)
myStudy.add(src)
src.run()
srcResult = src.getResult()


script = myStudy.getPythonScript()
print(script)
exec(script)
