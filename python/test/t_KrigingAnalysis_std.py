#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

ot.RandomGenerator.SetSeed(0)
ot.TBB_Disable()

myStudy = otguibase.OTStudy('myStudy')

## Model
xi1 = otguibase.Input('xi1', 0., '', ot.Uniform(0., 10.))
xi2 = otguibase.Input('xi2', 0., '', ot.Uniform(0., 10.))
y00 = otguibase.Output('fake_y0')
y00.setIsSelected(False)
y0 = otguibase.Output('y0')

formula_y00 = "xi1"
formula_y0 = "cos(0.5*xi1) + sin(xi2)"
model = otguibase.AnalyticalPhysicalModel('model', [xi1, xi2], [y00, y0], [formula_y00, formula_y0])
myStudy.add(model)

## Design of Experiment ##
aDesign = otguibase.DesignOfExperimentImplementation('design', model)
inputSample = ot.LHSExperiment(model.getComposedDistribution(), 10).generate()
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
openturns.testing.assert_almost_equal(aDesign.getOutputSample(), metaModel(inputSample), 3.0e-5, 3.0e-5)


## Design of Experiment ##
model.addOutput(otguibase.Output('y1'))
model.setFormula('y1', formula_y0)
model.addOutput(otguibase.Output('y2'))
model.setFormula('y2', '(cos(0.5*xi1) + sin(xi2))*2')
aDesign.run()

## Kriging ##
analysis2 = otguibase.KrigingAnalysis('kriging_1', aDesign)
analysis2.setLeaveOneOutValidation(True)
analysis2.setInterestVariables(['y1', 'y2'])
myStudy.add(analysis2)
print(analysis2)

analysis2.run()

metaModel2 = analysis2.getResult().getResultForVariable('y1').getMetaModel()
openturns.testing.assert_almost_equal(aDesign.getOutputSample().getMarginal(1), metaModel2(inputSample), 3.0e-5, 3.0e-5)
openturns.testing.assert_almost_equal(analysis2.getResult().getQ2LeaveOneOut(), [0.832278,0.832264], 1e-3, 1e-3)

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
