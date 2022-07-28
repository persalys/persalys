#!/usr/bin/env python

import openturns as ot
import openturns.testing
import persalys

#ot.Log.Show(ot.Log.DBG)
ot.TBB.Disable()
ot.RandomGenerator.SetSeed(0)
myStudy = persalys.Study('myStudy')

# Model
x1 = persalys.Input('x1', ot.Uniform(0., 10.))
x2 = persalys.Input('x2', ot.Uniform(0., 10.))
x3 = persalys.Input('x3', 0.5)
y00 = persalys.Output('fake_y0')
y00.setIsSelected(False)
y0 = persalys.Output('y0')

formula = ["0.5*x1 + x2+ 28*x3^2"] * 2
model = persalys.SymbolicPhysicalModel('model', [x1, x2, x3], [y00, y0], formula)
myStudy.add(model)

# Design of Experiment
aDesign = persalys.FixedDesignOfExperiment('design', model)
validationInputSample = ot.LHSExperiment(
    model.getDistribution(), 10).generate()
inputSample = ot.Sample(validationInputSample)
inputSample.stack(ot.Sample(10, [0.5]))
aDesign.setOriginalInputSample(inputSample)
myStudy.add(aDesign)

aDesign.run()

# LM
analysis = persalys.LinearRegressionAnalysis('lm_0', aDesign)
analysis.setDegree(2)
analysis.setInteraction(True)
myStudy.add(analysis)
print(analysis)

analysis.run()
print(analysis.getResult())
metaModel = analysis.getResult().getResultForVariable('y0').getMetaModel()

openturns.testing.assert_almost_equal(
    aDesign.getResult().getDesignOfExperiment().getOutputSample(), metaModel(validationInputSample), 3.0e-5, 3.0e-5)


# boston price model
datamodel = persalys.DataModel('datamodel', "Housing-prices-Boston.csv", range(13), [13])
myStudy.add(datamodel)
analysis2 = persalys.LinearRegressionAnalysis('lm_1', datamodel)
analysis2.setDegree(2)
analysis2.setInteraction(True)
myStudy.add(analysis2)
analysis2.run()
print(analysis2.getResult())
metaModel2 = analysis2.getResult().getResultForVariable('MEDV').getMetaModel()
f = ot.SymbolicFunction(datamodel.getInputSample().getDescription(), analysis2.getResult().getFormulas())
x = [1.5]*13
openturns.testing.assert_almost_equal(metaModel2(x), f(x))

# script
script = myStudy.getPythonScript()
print(script)
exec(script)
