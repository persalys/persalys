#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

# Model
dist_E = ot.Beta(0.93, 3.2, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000., 9000., 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 4., 3.1e2, 4.5e2)

E = otguibase.Input('E', 3e7, dist_E, 'Young\'s modulus (Pa)')
F = otguibase.Input('F', 3e4, dist_F, 'Force applied (N)')
L = otguibase.Input('L', 250, dist_L, 'Length (m4)')
I = otguibase.Input('I', 400, dist_I, 'Section modulus (m4)')
y = otguibase.Output('y', 'Vertical deviation (m)')

model = otguibase.SymbolicPhysicalModel(
    'myPhysicalModel', [E, F, L, I], [y], ['F*L^3/(3*E*I)'])
myStudy.add(model)

f = model.getFunction()
print(f([3e7, 3e4, 250, 400]))

# Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', model)
sobol.setMaximumConfidenceIntervalLength(-1)
sobol.setMaximumCalls(1000)
sobol.setReplicationSize(166)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

# SRC ##
src = otguibase.SRCAnalysis('mySRC', model, 1000)
myStudy.add(src)
src.run()
srcResult = src.getResult()


# Create the Spearman correlation matrix of the input random vector
RS = ot.CorrelationMatrix(2)
RS[0, 1] = -0.2
# Evaluate the correlation matrix of the Normal copula from RS
R = ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(RS)
# Create the Normal copula parametrized by R
copula = ot.NormalCopula(R)
model.setCopula(['L', 'I'], copula)

# Design of Experiment - Parametric analysis ##
aDesign = otguibase.GridDesignOfExperiment('aDesign', model)
aDesign.setLevels([2, 2, 2, 2])
myStudy.add(aDesign)
aDesign.run()

# Taylor Expansion ##
taylorExpansionsMoments = otguibase.TaylorExpansionMomentsAnalysis(
    'myTaylorExpansionMoments', model)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

# Monte Carlo ##
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', model)
montecarlo.setMaximumCalls(1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

# limit state ##
limitState = otguibase.LimitState('limitState1', model, 'y', ot.Greater(), 30.)
myStudy.add(limitState)

# Monte Carlo ##
montecarloSimu = otguibase.MonteCarloReliabilityAnalysis(
    'myMonteCarloSimu', limitState)
montecarloSimu.setMaximumCalls(10000)
myStudy.add(montecarloSimu)
montecarloSimu.run()
montecarloSimuResult = montecarloSimu.getResult()

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
