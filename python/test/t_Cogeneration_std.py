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

myPhysicalModel = otguibase.AnalyticalPhysicalModel('myPhysicalModel')

myPhysicalModel.addInput(Q)
myPhysicalModel.addInput(E)
myPhysicalModel.addInput(C)

myPhysicalModel.addOutput(Ep)

myStudy.addPhysicalModel(myPhysicalModel)

## Parametric analysis ##
#analysis = otguibase.ParametricAnalysis('myParametricAnalysis', myPhysicalModel)
#myStudy.addAnalysis(analysis)
#analysis.run()
#analysisResult = analysis.getResult()
outputSample = [[0.060036508072],
[0.0292238907867],
[0.0684295269203],
[0.0378920382755],
[0.0812679132055],
[0.0511512752497],
[0.0892876773294],
[0.0594339324804]]

## Comparaison
#openturns.testing.assert_almost_equal(outputSample, analysisResult.getResultSample(), 1e-16)

## Design of Experiment - Parametric analysis ##
aDesign = otguibase.DesignOfExperiment('aDesign', myPhysicalModel)
myStudy.addDesignOfExperiment(aDesign)
aDesign.eval()
openturns.testing.assert_almost_equal(outputSample, aDesign.getOutputSample(), 1e-16)

## Quadratic Cumul ##
quadraticCumul = otguibase.QuadraticCumulAnalysis('myQuadraticCumul', myPhysicalModel)
myStudy.addAnalysis(quadraticCumul)
quadraticCumul.run()
quadraticCumulResult = quadraticCumul.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.059730458221, quadraticCumulResult.getMeanFirstOrder()[0], 1e-13)

## Monte Carlo ##
montecarlo = otguibase.MonteCarloAnalysis('myMonteCarlo', myPhysicalModel, 1000)
myStudy.addAnalysis(montecarlo)
montecarlo.run()
montecarloResult = montecarlo.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.0598440092379, montecarloResult.getMean()[0], 1e-13)
openturns.testing.assert_almost_equal(0.0112354112196, montecarloResult.getStandardDeviation()[0], 1e-13)

meanCI = montecarloResult.getMeanConfidenceInterval()
openturns.testing.assert_almost_equal(0.0591476440319, meanCI.getLowerBound()[0], 1e-13)
openturns.testing.assert_almost_equal(0.0605403744439, meanCI.getUpperBound()[0], 1e-13)
stdCi = montecarloResult.getStdConfidenceInterval()
openturns.testing.assert_almost_equal(0.0107636626958, stdCi.getLowerBound()[0], 1e-13)
openturns.testing.assert_almost_equal(0.0117507273707, stdCi.getUpperBound()[0], 1e-13)

## Sobol ##
sobol = otguibase.SobolAnalysis('mySobol', myPhysicalModel)
myStudy.addAnalysis(sobol)
sobol.run()
sobolResult = sobol.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.589347, sobolResult.getFirstOrderIndices()[0][0], 1e-6)
openturns.testing.assert_almost_equal(0.0578996, sobolResult.getFirstOrderIndices()[0][1], 1e-6)
openturns.testing.assert_almost_equal(0.319232, sobolResult.getFirstOrderIndices()[0][2], 1e-5)
openturns.testing.assert_almost_equal(0.677013, sobolResult.getTotalOrderIndices()[0][0], 1e-6)
openturns.testing.assert_almost_equal(0.0430664, sobolResult.getTotalOrderIndices()[0][1], 1e-6)
openturns.testing.assert_almost_equal(0.309692, sobolResult.getTotalOrderIndices()[0][2], 1e-6)

## SRC ##
src = otguibase.SRCAnalysis('mySRC', myPhysicalModel)
myStudy.addAnalysis(src)
src.run()
srcResult = src.getResult()

# Comparaison
openturns.testing.assert_almost_equal(0.628946, srcResult.getIndices()[0][0], 1e-6)
openturns.testing.assert_almost_equal(0.0476118, srcResult.getIndices()[0][1], 1e-6)
openturns.testing.assert_almost_equal(0.318226, srcResult.getIndices()[0][2], 1e-6)

script = myStudy.dump()
print(script)
exec(script)
