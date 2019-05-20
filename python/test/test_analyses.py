#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import persalys
import numpy as np

ot.RandomGenerator.SetSeed(0)
ot.TBB_Disable()

myStudy = persalys.Study('myStudy')
persalys.Study.Add(myStudy)

# Models
x1 = persalys.Input('x1', 0.2, ot.Uniform(0., 10.))
x2 = persalys.Input('x2', 1.2, ot.Uniform(0., 10.))
x3 = persalys.Input('x3', 1.0)
fake_var = persalys.Output('fake_var')
fake_var.setIsSelected(False)
y0 = persalys.Output('y0')
fake_y0 = persalys.Output('fake_y0')
fake_y0.setIsSelected(False)
y1 = persalys.Output('y1')

# model 1 ##
formula_fake_var = 'x1'
formula_y0 = 'cos(0.5*x1) + sin(x2)'
formula_y1 = 'cos(0.5*x1) + sin(x2) + x3'
model1 = persalys.SymbolicPhysicalModel('model1', [x1, x2, x3], [fake_var, y0, fake_y0, y1], [
                                         formula_fake_var, formula_y0, formula_y0, formula_y1])
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.25
model1.setCopula(['x1', 'x2'], ot.NormalCopula(R))
myStudy.add(model1)

# model 2 ##
code = 'from math import cos, sin, sqrt\n\ndef _exec(x1, x2, x3):\n    y0 = cos(0.5*x1) + sin(x2) + sqrt(x3)\n    return y0\n'
model2 = persalys.PythonPhysicalModel('model2', [x1, x2, x3], [y0], code)
myStudy.add(model2)

# model 3 ##
filename = 'data.csv'
cDist = ot.ComposedDistribution(
    [ot.Normal(), ot.Gumbel(), ot.Normal(), ot.Uniform()],
                                ot.ComposedCopula([ot.IndependentCopula(2), ot.GumbelCopula()]))
sample = cDist.getSample(200)
sample.exportToCSVFile(filename, ' ')
model3 = persalys.DataModel(
    'model3', 'data.csv', [0, 2, 3], [1], ['x_0', 'x_2', 'x_3'], ['x_1'])
myStudy.add(model3)

# Designs of Experiment ##

# design 1 ##
ot.RandomGenerator.SetSeed(0)
design_1 = persalys.FixedDesignOfExperiment('design_1', model1)
inputSample = ot.LHSExperiment(ot.ComposedDistribution(
    [ot.Uniform(0., 10.), ot.Uniform(0., 10.)]), 10).generate()
inputSample.stack(ot.Sample(10, [0.5]))
design_1.setOriginalInputSample(inputSample)
design_1.run()
myStudy.add(design_1)

# design 2 ##
values = [0, 0, 1]
lowerBounds = [0.5, 0.5, 0.9]
upperBounds = [9.5, 9.5, 1.1]
levels = [7, 7, 1]
design_2 = persalys.GridDesignOfExperiment(
    'design_2', model1, lowerBounds, upperBounds, levels, values)
myStudy.add(design_2)

# design 3 ##
design_3 = persalys.ImportedDesignOfExperiment(
    'design_3', model1, 'data.csv', [0, 2, 3])
design_3.run()
myStudy.add(design_3)

# design 4 ##
design_4 = persalys.ProbabilisticDesignOfExperiment(
    'design_4', model1, 100, "MONTE_CARLO")
design_4.run()
myStudy.add(design_4)

# design 5 ##
design_5 = persalys.GridDesignOfExperiment('design_5', model2)
myStudy.add(design_5)

# design 6 ##
lowerBounds = [0., 0., -0.2]
upperBounds = [0., 0., 1.]
levels = [1, 1, 2]
design_6 = persalys.GridDesignOfExperiment(
    'design_6', model2, lowerBounds, upperBounds, levels)
myStudy.add(design_6)

# model 4 ##
model4 = persalys.DataModel(
    'model4', inputSample, design_1.getResult().getDesignOfExperiment().getOutputSample())
myStudy.add(model4)

# 0- models evaluations

# 0-a evaluation model1 ##
evaluation1 = persalys.ModelEvaluation('evaluation1', model1)
myStudy.add(evaluation1)

# 0-b evaluation model2 ##
evaluation2 = persalys.ModelEvaluation('evaluation2', model2)
myStudy.add(evaluation2)

# 1- meta model1 ##

# 1-a Kriging ##
kriging = persalys.KrigingAnalysis('kriging', design_4)
kriging.setBasis(ot.LinearBasisFactory(2).build())
kriging.setCovarianceModel(ot.MaternModel(2))
kriging.setTestSampleValidation(True)
kriging.setKFoldValidation(True)
kriging.setInterestVariables(['y0', 'y1'])
myStudy.add(kriging)

# 1-b Chaos ##
chaos1 = persalys.FunctionalChaosAnalysis('chaos_1', design_4)
chaos1.setChaosDegree(7)
chaos1.setSparseChaos(True)
chaos1.setTestSampleValidation(True)
chaos1.setKFoldValidation(True)
chaos1.setInterestVariables(['y1'])
myStudy.add(chaos1)

# 1-c Chaos ##
chaos2 = persalys.FunctionalChaosAnalysis('chaos_2', design_2)
chaos2.setChaosDegree(2)
chaos2.setSparseChaos(True)
myStudy.add(chaos2)

# 2- central tendancy ##

# 2-a Monte Carlo ##
monteCarlo = persalys.MonteCarloAnalysis('MonteCarlo', model1)
monteCarlo.setIsConfidenceIntervalRequired(False)
monteCarlo.setMaximumCoefficientOfVariation(-1.)
monteCarlo.setMaximumElapsedTime(1000)
monteCarlo.setMaximumCalls(1000)
monteCarlo.setBlockSize(100)
monteCarlo.setSeed(2)
monteCarlo.setInterestVariables(['y0', 'y1'])
myStudy.add(monteCarlo)

# 2-b Taylor Expansion ##
taylor = persalys.TaylorExpansionMomentsAnalysis('Taylor', model1)
taylor.setInterestVariables(['y0', 'y1'])
myStudy.add(taylor)

# 3- reliability ##

# limit state ##
limitState = persalys.LimitState(
    'aLimitState', model1, 'y1', ot.Greater(), 0.5)
myStudy.add(limitState)

optimAlgo = ot.AbdoRackwitz()
optimAlgo.setMaximumIterationNumber(150)
optimAlgo.setMaximumAbsoluteError(1e-3)

# 3-a Monte Carlo ##
monteCarloReliability = persalys.MonteCarloReliabilityAnalysis(
    'MonteCarloReliability', limitState)
monteCarloReliability.setMaximumCoefficientOfVariation(-1.)
monteCarloReliability.setMaximumElapsedTime(1000)
monteCarloReliability.setMaximumCalls(1000)
monteCarloReliability.setBlockSize(100)
monteCarloReliability.setSeed(2)
myStudy.add(monteCarloReliability)

# 3-b FORM IS ##
form_is = persalys.FORMImportanceSamplingAnalysis('FORM_IS', limitState)
form_is.setOptimizationAlgorithm(optimAlgo)
form_is.setMaximumCoefficientOfVariation(-1.)
form_is.setMaximumElapsedTime(1000)
form_is.setMaximumCalls(1000)
form_is.setBlockSize(100)
form_is.setSeed(2)
myStudy.add(form_is)

# 3-c FORM ##
form = persalys.FORMAnalysis('FORM', limitState)
form.setOptimizationAlgorithm(optimAlgo)
myStudy.add(form)

# 3-d SORM ##
sorm = persalys.SORMAnalysis('SORM', limitState)
sorm.setOptimizationAlgorithm(optimAlgo)
myStudy.add(sorm)

# 4- sensitivity ##

# 4-a Sobol ##
sobol = persalys.SobolAnalysis('Sobol', model1)
sobol.setMaximumConfidenceIntervalLength(-1)
sobol.setMaximumElapsedTime(1000)
sobol.setMaximumCalls(1000)
sobol.setReplicationSize(100)
sobol.setBlockSize(100)
sobol.setSeed(2)
sobol.setInterestVariables(['y0', 'y1'])
myStudy.add(sobol)

# 4-b SRC ##
src = persalys.SRCAnalysis('SRC', model1)
src.setSimulationsNumber(200)
src.setSeed(2)
src.setInterestVariables(['y0', 'y1'])
myStudy.add(src)

# 5- optimization ##
optim = persalys.OptimizationAnalysis('optim', model1, 'TNC')
optim.setInterestVariables(['y1'])
optim.setVariableInputs(['x1', 'x2'])
optim.setMaximumEvaluationNumber(150)
optim.setMaximumAbsoluteError(1e-6)
optim.setMaximumRelativeError(1e-6)
optim.setMaximumResidualError(1e-6)
optim.setMaximumConstraintError(1e-6)
myStudy.add(optim)

# 6- morris ##
try:
    morris = persalys.MorrisAnalysis('aMorris', model1)
    morris.setInterestVariables(['y0'])
    morris.setLevel(4)
    morris.setTrajectoriesNumber(10)
    morris.setSeed(2)
    myStudy.add(morris)
except:
    print("No Morris")

# 7- data analysis ##
dataAnalysis = persalys.DataAnalysis('DataAnalysis', model3)
myStudy.add(dataAnalysis)

# 8- Marginals inference ##
inference = persalys.InferenceAnalysis('inference', model3)
inference.setInterestVariables(['x_0', 'x_3'])
factories = [ot.NormalFactory(), ot.GumbelFactory()]
inference.setDistributionsFactories('x_3', factories)
inference.setLevel(0.1)
myStudy.add(inference)

# 9- Copula inference ##
copulaInference = persalys.CopulaInferenceAnalysis('copulaInference', model3)
factories = [ot.NormalCopulaFactory(), ot.GumbelCopulaFactory()]
copulaInference.setDistributionsFactories(['x_0', 'x_3'], factories)
myStudy.add(copulaInference)
