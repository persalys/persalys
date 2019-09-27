#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import persalys

ot.RandomGenerator.SetSeed(0)
ot.TBB_Disable()

myStudy = persalys.Study('myStudy')
persalys.Study.Add(myStudy)

# Models
x1 = persalys.Input('x1', 0.2)
x2 = persalys.Input('x2', 1.2)
x3 = persalys.Input('x3', 1.0)
fake_var = persalys.Output('fake_var')
fake_var.setIsSelected(False)
y0 = persalys.Output('y0')
fake_y0 = persalys.Output('fake_y0')
y1 = persalys.Output('y1')

# symboilc model ##
formula_fake_var = 'x1'
formula_y0 = 'cos(0.5*x1) + sin(x2)'
formula_y1 = 'cos(0.5*x1) + sin(x2) + x3'
symbolicModel = persalys.SymbolicPhysicalModel('symbolicModel', [x1, x2, x3], [fake_var, y0, fake_y0, y1], [
                                         formula_fake_var, formula_y0, formula_y0, formula_y1])

myStudy.add(symbolicModel)

# python model ##
code = 'from math import cos, sin, sqrt\n\ndef _exec(x1, x2, x3):\n    y0 = cos(0.5*x1) + sin(x2) + sqrt(x3)\n    return y0\n'
pythonModel = persalys.PythonPhysicalModel('pythonModel', [x1, x2, x3], [y0], code)
myStudy.add(pythonModel)

filename = 'data.csv'
cDist = ot.ComposedDistribution([ot.Normal(), ot.Gumbel(), ot.Normal(), ot.Uniform()],
                                ot.ComposedCopula([ot.IndependentCopula(2), ot.GumbelCopula()]))
sample = cDist.getSample(200)
sample.exportToCSVFile(filename, ' ')

# Designs of Experiment ##

# fixed design ##
ot.RandomGenerator.SetSeed(0)
fixedDesign = persalys.FixedDesignOfExperiment('fixedDesign', symbolicModel)
inputSample = ot.LHSExperiment(ot.ComposedDistribution([ot.Uniform(0., 10.), ot.Uniform(0., 10.)]), 10).generate()
inputSample.stack(ot.Sample(10, [0.5]))
fixedDesign.setOriginalInputSample(inputSample)
fixedDesign.run()
myStudy.add(fixedDesign)

# grid ##
values = [[0.5+i*1.5 for i in range(7)], [0.5+i*1.5 for i in range(7)], [1]]
grid = persalys.GridDesignOfExperiment('grid', symbolicModel, values)
myStudy.add(grid)

# importDesign ##
importDesign = persalys.ImportedDesignOfExperiment('importDesign', symbolicModel, 'data.csv', [0, 2, 3])
importDesign.run()
myStudy.add(importDesign)

# onePointDesign ##
onePointDesign = persalys.GridDesignOfExperiment('onePointDesign', pythonModel)
myStudy.add(onePointDesign)

# twoPointsDesign ##
twoPointsDesign = persalys.GridDesignOfExperiment('twoPointsDesign', pythonModel, [[0.2], [1.2], [-0.2, 1.]])
myStudy.add(twoPointsDesign)

# fixed DataModel ##
fixedDataModel = persalys.DataModel('fixedDataModel', fixedDesign.getOriginalInputSample(), fixedDesign.getResult().getDesignOfExperiment().getOutputSample())
myStudy.add(fixedDataModel)

# imported DataModel ##
importDataModel = persalys.DataModel('importDataModel', 'data.csv', [0, 2, 3], [1], ['x_0', 'x_2', 'x_3'], ['x_1'])
myStudy.add(importDataModel)

# model evaluation
evaluation1 = persalys.ModelEvaluation('evaluation1', symbolicModel)
myStudy.add(evaluation1)

# morris ##
try:
    morris = persalys.MorrisAnalysis('aMorris', symbolicModel)
    morris.setInterestVariables(['y0'])
    morris.setLevel(4)
    morris.setTrajectoriesNumber(10)
    morris.setSeed(2)
    myStudy.add(morris)
except:
    print("No Morris")

# optimization ##
optim = persalys.OptimizationAnalysis('optim', symbolicModel, 'TNC')
optim.setInterestVariables(['y1'])
optim.setVariableInputs(['x1', 'x2'])
optim.setMaximumEvaluationNumber(150)
optim.setMaximumAbsoluteError(1e-6)
optim.setMaximumRelativeError(1e-6)
optim.setMaximumResidualError(1e-6)
optim.setMaximumConstraintError(1e-6)
myStudy.add(optim)

# calibration ##

# a- observations
nbObs = 100
inObs = ot.Uniform(0., 10.).getSample(nbObs)
inObs.setDescription(['x1'])

y0Noise = ot.Normal(0, 0.1).getSample(nbObs)
y0Sample = ot.ParametricFunction(symbolicModel.getFunction('y0'), [1, 2], [1.2, 1.])(inObs)

y0Obs = y0Sample + y0Noise
y0Obs.setDescription(['y0'])
obs = persalys.Observations("observations", symbolicModel, inObs, y0Obs)
myStudy.add(obs)

# b- calibrationAnalysis
calibration = persalys.CalibrationAnalysis('calibration', obs)
calibration.setCalibratedInputs(['x2'], ot.Dirac([1.2]), ['x3'], [1.1])
calibration.setMethodName('GaussianNonlinear')
sigma = 0.15
errorCovariance = ot.CovarianceMatrix(1)
errorCovariance[0, 0] = sigma**2
calibration.setErrorCovariance(errorCovariance)
calibration.setBootStrapSize(25)
calibration.setConfidenceIntervalLength(0.99)

optimAlgo = calibration.getOptimizationAlgorithm()
optimAlgo.setMaximumEvaluationNumber(50)
optimAlgo.setMaximumAbsoluteError(1e-6)
optimAlgo.setMaximumRelativeError(1e-6)
optimAlgo.setMaximumResidualError(1e-6)
optimAlgo.setMaximumConstraintError(1e-6)
calibration.setOptimizationAlgorithm(optimAlgo)

myStudy.add(calibration)


