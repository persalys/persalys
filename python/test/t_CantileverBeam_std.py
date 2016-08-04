#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')
otguibase.OTStudy.Add(myStudy)

E = otguibase.Input('E', 3e7, 'Young s modulus', ot.Beta(0.93, 3.2, 2.8e7, 4.8e7))
F = otguibase.Input('F', 3e4, 'Charge applied', ot.LogNormal(30000., 9000., 15000, ot.LogNormal.MUSIGMA))
L = otguibase.Input('L', 250, 'Length', ot.Uniform(250, 260))
I = otguibase.Input('I', 400, 'Section modulus', ot.Beta(2.5, 4., 3.1e2, 4.5e2))
y = otguibase.Output('y', 0., 'deviation', 'F*L^3/(3*E*I)')

myPhysicalModel = otguibase.AnalyticalPhysicalModel('myPhysicalModel')
myPhysicalModel.addInput(E)
myPhysicalModel.addInput(F)
myPhysicalModel.addInput(L)
myPhysicalModel.addInput(I)
myPhysicalModel.addOutput(y)

f = myPhysicalModel.getFunction()
print(f([3e7, 3e4, 250, 400]))

myStudy.add(myPhysicalModel)


## Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', myPhysicalModel)
sobol.setMaximumCoefficientOfVariation(-1)
sobol.setMaximumCalls(1000)
sobol.setBlockSize(1000)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()


# Create the Spearman correlation matrix of the input random vector
RS = ot.CorrelationMatrix(4)
RS[2,3] = -0.2
# Evaluate the correlation matrix of the Normal copula from RS
R = ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(RS)
# Create the Normal copula parametrized by R
copula = ot.NormalCopula(R)
myPhysicalModel.setCopula(copula)

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
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', myPhysicalModel)
montecarlo.setMaximumCalls(1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

## SRC ##
src = otguibase.SRCAnalysis('mySRC', myPhysicalModel, 1000)
myStudy.add(src)
src.run()
srcResult = src.getResult()

## limit state ##
limitState = otguibase.LimitState('limitState1', myPhysicalModel, 'y', ot.Greater(), 30.)
myStudy.add(limitState)

## Monte Carlo ##
montecarloSimu = otguibase.MonteCarloReliabilityAnalysis('myMonteCarloSimu', limitState)
montecarloSimu.setMaximumCalls(10000)
myStudy.add(montecarloSimu)
montecarloSimu.run()
montecarloSimuResult = montecarloSimu.getResult()

script = myStudy.getPythonScript()
print(script)
exec(script)
