#!/usr/bin/env python

import openturns as ot
import openturns.testing
import persalys

myStudy = persalys.Study("myStudy")

# Model
dist_E = ot.Beta(0.93, 3.2, 2.8e7, 4.8e7)
dist_F = ot.LogNormalMuSigma(30000.0, 9000.0, 15000).getDistribution()
dist_L = ot.Uniform(250, 260)
dist_I = ot.Beta(2.5, 4.0, 3.1e2, 4.5e2)

E = persalys.Input("E", 3e7, dist_E, "Young's modulus (Pa)")
F = persalys.Input("F", 3e4, dist_F, "Force applied (N)")
L = persalys.Input("L", 250, dist_L, "Length (m4)")
II = persalys.Input("I", 400, dist_I, "Section modulus (m4)")
y = persalys.Output("y", "Vertical deviation (m)")

model = persalys.SymbolicPhysicalModel(
    "myPhysicalModel", [E, F, L, II], [y], ["F*L^3/(3*E*I)"]
)
myStudy.add(model)

f = model.getFunction()
print(f([3e7, 3e4, 250, 400]))

# Sobol ##
sobol = persalys.SobolAnalysis("mySobol", model)
sobol.setMaximumConfidenceIntervalLength(-1)
sobol.setMaximumCalls(1000)
sobol.setReplicationSize(166)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

# SRC ##
src = persalys.SRCAnalysis("mySRC", model, 1000)
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
model.setCopula(["L", "I"], copula)

# Design of Experiment - Parametric analysis ##
bounds = persalys.GridDesignOfExperiment.GetDefaultBounds(model)
values = [[bounds.getLowerBound()[i], bounds.getUpperBound()[i]] for i in range(4)]
aDesign = persalys.GridDesignOfExperiment("aDesign", model, values)
myStudy.add(aDesign)
aDesign.run()

# Taylor Expansion ##
taylorExpansionsMoments = persalys.TaylorExpansionMomentsAnalysis(
    "myTaylorExpansionMoments", model
)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

# Monte Carlo ##
montecarlo = persalys.MonteCarloAnalysis("myMonteCarlo", model)
montecarlo.setMaximumCalls(1000)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

meanCI = montecarloResult.getMeanConfidenceInterval()
stdCi = montecarloResult.getStdConfidenceInterval()

# limit state ##
limitState = persalys.LimitState("limitState1", model, "y", ot.Greater(), 30.0)
myStudy.add(limitState)

# Monte Carlo ##
montecarloSimu = persalys.MonteCarloReliabilityAnalysis("myMonteCarloSimu", limitState)
montecarloSimu.setMaximumCalls(10000)
myStudy.add(montecarloSimu)
montecarloSimu.run()
montecarloSimuResult = montecarloSimu.getResult()

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
