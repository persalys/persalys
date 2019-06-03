#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import persalys

myStudy = persalys.Study('myStudy')

# Model
X0 = persalys.Input('X0', 1, ot.Normal(), 'aDescription')
X1 = persalys.Input('X1', 2)
Y0 = persalys.Output('Y0', 'output_1')
inputs = [X0, X1]
outputs = [Y0]
formulas = ['sin(X0)+8*X1']
model = persalys.SymbolicPhysicalModel(
    'aModelPhys', inputs, outputs, formulas)
myStudy.add(model)
print(model)

# get attributs values
print('function=', model.getFunction())
print('formulas=', model.getFormulas())
print('formula_Y0=', model.getFormula('Y0'))
print('stochastic var=', model.getStochasticInputNames())
print('distribution=', model.getDistribution())
print('copula=', model.getCopula())
print('output_YO=', model.getOutputByName('Y0'))
print('input_XO=', model.getInputByName('X0'))
print('inputs names=', model.getInputNames())
print('outputs names=', model.getOutputNames())
print('hasY0', model.hasOutputNamed('Y0'))
print('hasX0', model.hasInputNamed('X0'))

# set attributs values
# in
model.setInputs(inputs)
model.setOutputs(outputs)
model.setDistribution('X1', ot.LogNormal())
model.setFiniteDifferenceStep('X1', 1e-5)
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.25
model.setCopula(['X0', 'X1'], ot.NormalCopula(R))
print('inputs=', model.getInputs())
print('stochastic var=', model.getStochasticInputNames())
print('distribution=', model.getDistribution())
print('copula=', model.getCopula())
# out
model.setFormulas(['sin(X0)+8*X1+0.5'])
print('outputs=', model.getOutputs())

# add variables
# in
X2 = persalys.Input('X2', 10)
model.addInput(X2)
model.setInputName('X2', 'X_2')
print('inputs=', model.getInputs())
print('copula=', model.getCopula())
X3 = persalys.Input('X3', 10, 'aDescription')
model.addInput(X3)
X4 = persalys.Input('X4', ot.Normal())
model.addInput(X4)
X5 = persalys.Input('X5', ot.Normal(), 'aDescription')
model.addInput(X5)
print('stochastic var=', model.getStochasticInputNames())
print('distribution=', model.getDistribution())
print('copula=', model.getCopula())
# out
model.addOutput(persalys.Output('Y1'))
model.setFormula('Y1', 'sin(X0)+8*X1+X2')
model.setOutputName('Y1', 'Y_1')
model.selectOutput('Y_1', False)
print('formula_Y1=', model.getFormula('Y_1'))
print('outputs=', model.getOutputs())
print('selected outputs=', model.getSelectedOutputsNames())

# remove variables
model.removeInput('X_2')
model.removeInput('X3')
model.removeOutput('Y_1')
print('inputs=', model.getInputs())
print('outputs=', model.getOutputs())

# script
script = model.getPythonScript()
print('script=', script)
exec(script)
