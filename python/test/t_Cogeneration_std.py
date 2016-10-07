#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy('myStudy')

Q = otguibase.Input('Q', 0., 'Primary energy', ot.Normal(10200, 100))
E = otguibase.Input('E', 0., 'Produced electric energy', ot.Normal(3000, 15))
C = otguibase.Input('C', 0., 'Valued thermal energy', ot.Normal(4000, 60))

Ep = otguibase.Output('Ep', 0., 'Primary energy savings', '1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))')

myPhysicalModel = otguibase.AnalyticalPhysicalModel('myPhysicalModel', [Q, E, C], [Ep])

myStudy.add(myPhysicalModel)

outputSample = [[0.060036508072],
[0.0292238907867],
[0.0684295269203],
[0.0378920382755],
[0.0812679132055],
[0.0511512752497],
[0.0892876773294],
[0.0594339324804]]


## Design of Experiment - Parametric analysis ##
aDesign = otguibase.DesignOfExperiment('aDesign', myPhysicalModel)
myStudy.add(aDesign)
aDesign.run()

# Comparaison
openturns.testing.assert_almost_equal(outputSample, aDesign.getOutputSample(), 1e-16)

## Quadratic Cumul ##
taylorExpansionsMoments = otguibase.TaylorExpansionMomentsAnalysis('myTaylorExpansionMoments', myPhysicalModel)
myStudy.add(taylorExpansionsMoments)
taylorExpansionsMoments.run()
taylorExpansionsMomentsResult = taylorExpansionsMoments.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.059730458221, taylorExpansionsMomentsResult.getMeanFirstOrder()[0], 1e-13)

## Monte Carlo ##
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', myPhysicalModel)
montecarlo.setMaximumCalls(1000)
montecarlo.setMaximumCoefficientOfVariation(-1)
myStudy.add(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.0597109963361, montecarloResult.getMean()[3], 1e-13)
openturns.testing.assert_almost_equal(0.0114128746587, montecarloResult.getStandardDeviation()[3], 1e-13)

meanCI = montecarloResult.getMeanConfidenceInterval()
openturns.testing.assert_almost_equal(0.0590036320343, meanCI.getLowerBound()[3], 1e-13)
openturns.testing.assert_almost_equal(0.0604183606379, meanCI.getUpperBound()[3], 1e-13)
stdCi = montecarloResult.getStdConfidenceInterval()
openturns.testing.assert_almost_equal(0.0109336748621, stdCi.getLowerBound()[3], 1e-13)
openturns.testing.assert_almost_equal(0.0119363302339, stdCi.getUpperBound()[3], 1e-13)

## Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', myPhysicalModel)
sobol.setMaximumCoefficientOfVariation(-1)
sobol.setMaximumCalls(1000)
sobol.setBlockSize(1000)
myStudy.add(sobol)
sobol.run()
sobolResult = sobol.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.66706589316, sobolResult.getFirstOrderIndices()[0][0], 1e-11)
openturns.testing.assert_almost_equal(0.0544481329747, sobolResult.getFirstOrderIndices()[0][1], 1e-13)
openturns.testing.assert_almost_equal(0.33361028403, sobolResult.getFirstOrderIndices()[0][2], 1e-11)
openturns.testing.assert_almost_equal(0.608839559982, sobolResult.getTotalIndices()[0][0], 1e-12)
openturns.testing.assert_almost_equal(0.0574755596289, sobolResult.getTotalIndices()[0][1], 1e-13)
openturns.testing.assert_almost_equal(0.289572736872, sobolResult.getTotalIndices()[0][2], 1e-12)

## SRC ##
src = otguibase.SRCAnalysis('mySRC', myPhysicalModel)
myStudy.add(src)
src.run()
srcResult = src.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.632244, srcResult.getIndices()[0][0], 1e-5)
openturns.testing.assert_almost_equal(0.0478614, srcResult.getIndices()[0][1], 1e-5)
openturns.testing.assert_almost_equal(0.319895, srcResult.getIndices()[0][2], 1e-5)

## Chaos ##
values = [10200, 3000, 4000]
lowerBounds = [10035.5, 2975.33, 3901.31]
upperBounds = [10364.5, 3024.67, 4098.69]
levels = [10, 10, 10]
design_0 = otguibase.FixedDesignOfExperiment('design_0', myPhysicalModel, lowerBounds, upperBounds, levels, values)
design_0.run()
myStudy.add(design_0)

chaos_0 = otguibase.FunctionalChaosAnalysis('chaos_0', design_0)
chaos_0.setChaosDegree(2)
chaos_0.setSparseChaos(False)
myStudy.add(chaos_0)
chaos_0.run()
chaosResult = chaos_0.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.6356916720224053, chaosResult.getSobolResult().getFirstOrderIndices()[0][0], 1e-16)
openturns.testing.assert_almost_equal(0.04806204987068495, chaosResult.getSobolResult().getFirstOrderIndices()[0][1], 1e-17)
openturns.testing.assert_almost_equal(0.31620207904361813, chaosResult.getSobolResult().getFirstOrderIndices()[0][2], 1e-17)
openturns.testing.assert_almost_equal(0.6357266809805613, chaosResult.getSobolResult().getTotalIndices()[0][0], 1e-16)
openturns.testing.assert_almost_equal(0.04807585948286413, chaosResult.getSobolResult().getTotalIndices()[0][1], 1e-17)
openturns.testing.assert_almost_equal(0.3162416585998657, chaosResult.getSobolResult().getTotalIndices()[0][2], 1e-17)

script = myStudy.getPythonScript()
print(script)
exec(script)
