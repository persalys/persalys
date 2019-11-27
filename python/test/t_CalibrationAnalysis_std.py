#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import persalys

ot.RandomGenerator.SetSeed(0)

myStudy = persalys.Study('myStudy')
persalys.Study.Add(myStudy)

R = persalys.Input('R', 700e6, ot.LogNormalMuSigma(750e6, 11e6).getDistribution(), 'Parameter R')
C = persalys.Input('C', 2500e6, ot.Normal(2750e6, 250e6), 'Parameter C')
gamma = persalys.Input('gam', 8., ot.Normal(10, 2), 'Parameter gamma')
dist_strain = ot.Uniform(0, 0.07)
strain = persalys.Input('strain', 0., dist_strain, 'Strain')
sigma = persalys.Output('sigma', 'Stress (Pa)')
fakeSigma = persalys.Output('fakeSigma', 'Stress (Pa)')

formula = ['R + C * (1 - exp(-gam * strain))', '2*(R + C * (1 - exp(-gam * strain)))']
model = persalys.SymbolicPhysicalModel('model1', [R, C, gamma, strain], [sigma, fakeSigma], formula)
myStudy.add(model)

# generate observations
nbObs = 100
strainObs = dist_strain.getSample(nbObs)
strainObs.setDescription(['strain'])

stressSampleNoise = ot.Normal(0., 40.e6).getSample(nbObs)
stressSample = ot.ParametricFunction(model.getFunction('sigma'), [0, 1, 2], [750e6, 2750e6, 10.])(strainObs)

stressObs = stressSample + stressSampleNoise
stressObs.setDescription(['sigma'])
observations = persalys.Observations('obs1', model, strainObs, stressObs)

myStudy.add(observations)

# Least Squares linear
analysis = persalys.CalibrationAnalysis('myAnalysis', observations)
analysis.run()
myStudy.add(analysis)
print("analysis=", analysis)
print("result=", analysis.getResult())

# Least Squares Non linear
analysis2 = persalys.CalibrationAnalysis('myAnalysis2', observations)
analysis2.setMethodName('LeastSquaresNonlinear')
analysis2.setCalibratedInputs(['R', 'C'], ot.Dirac([700e6, 2400e6 ]), ['gam'], [7.])
analysis2.run()
myStudy.add(analysis2)
print("analysis=", analysis2)

# Gaussian linear calibration
analysis3 = persalys.CalibrationAnalysis('myAnalysis3', observations)
sigmaStress = 1.e7 # (Pa)
errorCovariance = ot.CovarianceMatrix(1)
errorCovariance[0,0] = sigmaStress**2
analysis3.setMethodName('GaussianLinear')
analysis3.setErrorCovariance(errorCovariance)
analysis3.run()
myStudy.add(analysis3)
print("analysis=", analysis3)

# Gaussian non linear calibration
analysis4 = persalys.CalibrationAnalysis('myAnalysis4', observations)
analysis4.setMethodName('GaussianNonlinear')
analysis4.setErrorCovariance(errorCovariance)
analysis4.setBootStrapSize(50)
analysis4.setConfidenceIntervalLength(0.99)
optimizationAlgo = analysis4.getOptimizationAlgorithm()
optimizationAlgo.setMaximumEvaluationNumber(50)
optimizationAlgo.setMaximumAbsoluteError(1e-6)
analysis4.setOptimizationAlgorithm(optimizationAlgo)
analysis4.run()
myStudy.add(analysis4)
print("analysis=", analysis4)


# analysis check

# - check two outputs
stressObs.stack(stressObs * 2)
stressObs.setDescription(['sigma', 'fakeSigma'])

observations2 = persalys.Observations('obs2', model, strainObs, stressObs)

analysis5 = persalys.CalibrationAnalysis('myAnalysis5', observations2)
analysis5.run()

fakeIn = persalys.Input('fakeIn')
model.addInput(fakeIn)

# - check fixed input order
analysis6 = persalys.CalibrationAnalysis('myAnalysis6', observations)
analysis6.setCalibratedInputs(['R', 'C'], ot.Dirac([700e6, 2500e6 ]), ['fakeIn', 'gam'], [0., 7.])
analysis6.run()

# - check calibrated input order
analysis7 = persalys.CalibrationAnalysis('myAnalysis7', observations)
analysis7.setCalibratedInputs(['C', 'R'], ot.Dirac([2500e6, 700e6]), ['gam', 'fakeIn'], [7., 0.])
analysis7.run()

# - check observed input order
strainObs2 = ot.Normal().getSample(nbObs)
strainObs2.stack(strainObs)
strainObs2.setDescription(['fakeIn', 'strain'])

observations3 = persalys.Observations('obs3', model, strainObs2, stressObs)

analysis8 = persalys.CalibrationAnalysis('myAnalysis8', observations3)
analysis8.setCalibratedInputs(['R', 'C'], ot.Dirac([700e6, 2500e6 ]), ['gam'], [7.])
analysis8.run()

# - check exceptions
try:
  analysis6.setCalibratedInputs(['R', 'C'], ot.Dirac([700e6, 2400e6 ]), ['gam', 'fakeIn'], [7., 0.])
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  analysis6.setCalibratedInputs(['R', 'C', 'fakeIn'], ot.Dirac([700e6, 2400e6, 4.]), ['gam'], [7.])
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  analysis6.setCalibratedInputs(['R', 'C'], ot.Dirac([700e6, 2400e6, 4.]), ['gam', 'C'], [7., 1e9])
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))
try:
  analysis6.setCalibratedInputs(['R', 'C'], ot.Normal(3), ['gam'], [7.])
except Exception as e:
  print("InvalidArgumentException occured: %s" % ("InvalidArgumentException" in str(e)))


print("\nthetaMAP=", analysis.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis.getResult().getConfidenceInterval())

#print("\nthetaMAP=", analysis2.getResult().getCalibrationResult().getParameterMAP())
#print("thetaMAP CI=\n", analysis2.getResult().getConfidenceInterval())
ott.assert_almost_equal(analysis2.getResult().getCalibrationResult().getParameterMAP(), [7.73108e+08,3.55285e+09], 1e-2, 1)

print("\nthetaMAP=", analysis3.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis3.getResult().getConfidenceInterval())

#print("\nthetaMAP=", analysis4.getResult().getCalibrationResult().getParameterMAP())
#print("thetaMAP CI=\n", analysis4.getResult().getConfidenceInterval())
ott.assert_almost_equal(analysis4.getResult().getCalibrationResult().getParameterMAP(), [7.47517e+08,2.83374e+09,9.58942], 1e-2, 1)

print("\nthetaMAP=", analysis5.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis5.getResult().getConfidenceInterval())
print("\nthetaMAP=", analysis6.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis6.getResult().getConfidenceInterval())
print("\nthetaMAP=", analysis7.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis7.getResult().getConfidenceInterval())
print("\nthetaMAP=", analysis8.getResult().getCalibrationResult().getParameterMAP())
print("thetaMAP CI=\n", analysis8.getResult().getConfidenceInterval())


# script
script = myStudy.getPythonScript()
print(script)
exec(script)
