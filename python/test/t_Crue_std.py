#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import openturns.testing
import persalys

myStudy = persalys.Study('myStudy')

# Model
dist_Q = ot.Gumbel(1. / 558., 1013.)
dist_Ks = ot.TruncatedDistribution(
    ot.Normal(30.0, 7.5), 0, ot.TruncatedDistribution.LOWER)
dist_Zv = ot.Uniform(49.0, 51.0)
dist_Zm = ot.Uniform(54.0, 56.0)

Q = persalys.Input('Q', 1000., dist_Q, 'Débit maximal annuel (m3/s)')
Ks = persalys.Input('Ks', 30., dist_Ks, 'Strickler (m^(1/3)/s)')
Zv = persalys.Input('Zv', 50., dist_Zv, 'Côte de la rivière en aval (m)')
Zm = persalys.Input('Zm', 55., dist_Zm, 'Côte de la rivière en amont (m)')
S = persalys.Output('S', 'Surverse (m)')

model = persalys.SymbolicPhysicalModel('myPhysicalModel', [Q, Ks, Zv, Zm], [
                                        S], ['(Q/(Ks*300.*sqrt((Zm-Zv)/5000)))^(3.0/5.0)+Zv-55.5-3.'])
myStudy.add(model)

# limit state ##
limitState = persalys.LimitState('limitState1', model, 'S', ot.Greater(), -1.)
myStudy.add(limitState)

# Monte Carlo ##
montecarlo = persalys.MonteCarloReliabilityAnalysis(
    'myMonteCarlo', limitState)
montecarlo.setMaximumCalls(10000)
montecarlo.setMaximumCoefficientOfVariation(0.01)
montecarlo.setBlockSize(1)
myStudy.add(montecarlo)

montecarlo.run()
montecarloResult = montecarlo.getResult()

# Comparaison
openturns.testing.assert_almost_equal(montecarloResult.getSimulationResult().getProbabilityEstimate(), 9.999999999999946e-05, 1e-6)

# FORM-IS ##
formIS = persalys.FORMImportanceSamplingAnalysis('myformIS', limitState)
formIS.setMaximumCoefficientOfVariation(0.01)
formIS.setMaximumCalls(10000)
formIS.setBlockSize(1000)
myStudy.add(formIS)

formIS.run()
formISResult = formIS.getResult()

# Comparaison
openturns.testing.assert_almost_equal(formISResult.getSimulationResult().getProbabilityEstimate(), 0.00022, 1e-5, 1e-5)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
