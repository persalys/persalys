#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

ot.RandomGenerator.SetSeed(0)
ot.TBB_Disable()

myStudy = otguibase.OTStudy('myStudy')

## Model
xi1 = otguibase.Input('xi1', ot.Uniform(0., 10.))
xi2 = otguibase.Input('xi2', ot.Uniform(0., 10.))
xi3 = otguibase.Input('xi3', 0.5)
y00 = otguibase.Output('fake_y0')
y00.setIsSelected(False)
y0 = otguibase.Output('y0')

formula_y00 = "xi1"
formula_y0 = "cos(0.5*xi1) + sin(xi2)"
model = otguibase.SymbolicPhysicalModel('model', [xi1, xi2, xi3], [y00, y0], [formula_y00, formula_y0])
myStudy.add(model)

## Design of Experiment ##
aDesign = otguibase.DesignOfExperimentImplementation('design', model)
validationInputSample = ot.LHSExperiment(model.getComposedDistribution(), 10).generate()
inputSample = ot.NumericalSample(validationInputSample)
inputSample.stack(ot.NumericalSample(10, [0.5]))
aDesign.setInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

## Kriging ##
analysis = otguibase.KrigingAnalysis('kriging_0', aDesign)
analysis.setBasis(ot.LinearBasisFactory(2).build())
analysis.setCovarianceModel(ot.MaternModel(2))
myStudy.add(analysis)
print(analysis)

analysis.run()

metaModel = analysis.getResult().getResultForVariable('y0').getMetaModel()
openturns.testing.assert_almost_equal(aDesign.getOutputSample(), metaModel(validationInputSample), 3.0e-5, 3.0e-5)

## Design of Experiment ##
model.addOutput(otguibase.Output('y1'))
model.setFormula('y1', formula_y0 + ' + xi3')
aDesign.run()

## Kriging ##
analysis2 = otguibase.KrigingAnalysis('kriging_1', aDesign)
analysis2.setLeaveOneOutValidation(True)
analysis2.setInterestVariables(['y1', 'y0'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

result2 = analysis2.getResult()
metaModel2 = result2.getResultForVariable('y1').getMetaModel()
openturns.testing.assert_almost_equal(aDesign.getOutputSample().getMarginal(1), metaModel2(validationInputSample), 3.0e-5, 3.0e-5)
openturns.testing.assert_almost_equal(result2.getQ2LeaveOneOut(), [0.832447,0.832278], 1e-3, 1e-3)

## Kriging ##
analysis3 = otguibase.KrigingAnalysis('kriging_2', aDesign)
analysis3.setOptimizeParameters(False)
analysis3.setInterestVariables(['y0'])
myStudy.add(analysis3)
print(analysis3)

analysis3.run()

result3 = analysis3.getResult()
print("result=", result3)

metaModel3 = result3.getResultForVariable('y0').getMetaModel()
openturns.testing.assert_almost_equal(aDesign.getOutputSample().getMarginal(0), metaModel3(validationInputSample), 3.0e-5, 3.0e-5)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)