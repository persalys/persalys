#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import otguibase

myStudy = otguibase.OTStudy("myStudy")

Q = otguibase.Input("Q", 0., "Primary energy", ot.Normal(10200, 100))
E = otguibase.Input("E", 0., "Produced electric energy", ot.Normal(3000, 15))
C = otguibase.Input("C", 0., "Valued thermal energy", ot.Normal(4000, 60))

Ep = otguibase.Output("Ep", 0., "Primary energy savings", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))")

myPhysicalModel = otguibase.PhysicalModel("myPhysicalModel")

myPhysicalModel.addInput(Q)
myPhysicalModel.addInput(E)
myPhysicalModel.addInput(C)

myPhysicalModel.addOutput(Ep)

myStudy.addPhysicalModel(myPhysicalModel)

# Parametric analysis
calculus = otguibase.ParametricCalculus("myParametricCalculus", myPhysicalModel)
myStudy.addCalculus(calculus)
calculus.run()
resultSample = calculus.getResult().getResultSample()
print(calculus.getInputSample())


script = myStudy.dump()
print(script)


# Reference

def functionCogeneration(X):
    tauelec=0.54
    tauth=0.8
    tau=0.05
    Q, E, C = X
    Ep=1.-Q/(E/(1.-tau)/tauelec+C/tauth)
    return [Ep]

f = ot.PythonFunction(3, 1, functionCogeneration)

Q = ot.Normal(10200,100)
E = ot.Normal(3000,15)
C = ot.Normal(4000,60)
distributions = [Q, E, C]

scale=[]
transvec=[]
levels=[]
for i in range(3):
    a=distributions[i].computeQuantile(0.05)
    b=distributions[i].computeQuantile(0.95)
    scale.append(b[0]-a[0])
    transvec.append(a[0])
    levels.append(0)

box = ot.Box(levels)
inputSample = box.generate()
inputSample*=scale
inputSample+=transvec

outputSample=f(inputSample)

# Comparaison
openturns.testing.assert_almost_equal(outputSample, resultSample, 1e-16)
