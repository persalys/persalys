#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

## Model
Q = otguibase.Input('Q', 0., ot.TruncatedDistribution(ot.Gumbel(1./558., 1013.), 0, ot.TruncatedDistribution.LOWER), 'Q (m3/s)')
Ks = otguibase.Input('Ks', 0., ot.TruncatedDistribution(ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER), 'Ks (m^(1/3)/s)')
Zv = otguibase.Input('Zv', 0., ot.Uniform(49.0, 51.0), 'Zv (m)')
Zm = otguibase.Input('Zm', 0., ot.Uniform(54.0, 56.0), 'Zm (m)')
S = otguibase.Output('S')

model = otguibase.AnalyticalPhysicalModel('myPhysicalModel', [Q, Ks, Zv, Zm], [S], ['(Q/(Ks*300.*sqrt((Zm-Zv)/5000)))^(3.0/5.0)+Zv-55.5-3.'])
myStudy.add(model)

## limit state ##
limitState = otguibase.LimitState('limitState1', model, 'S', ot.Greater(), 0.)
myStudy.add(limitState)

## Monte Carlo ##
montecarlo = otguibase.MonteCarloReliabilityAnalysis('myMonteCarlo', limitState)
montecarlo.setMaximumCalls(10000)
myStudy.add(montecarlo)

montecarlo.run()
montecarloResult = montecarlo.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.0006, montecarloResult.getSimulationResult().getProbabilityEstimate(), 1e-16)

## FORM-IS ##
formIS = otguibase.FORMImportanceSamplingAnalysis('myformIS', limitState)
formIS.setMaximumCoefficientOfVariation(0.01)
formIS.setMaximumCalls(10000)
formIS.setBlockSize(1000)
myStudy.add(formIS)

formIS.run()
formISResult = formIS.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.000634097, formISResult.getSimulationResult().getProbabilityEstimate(), 1e-16)

## script
script = myStudy.getPythonScript()
print(script)
exec(script)
