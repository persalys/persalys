#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase
from math import pi

code = """
import openturns as ot
def _exec(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20):
    ot.RandomGenerator.SetSeed(123)
    b0 = ot.DistFunc.rNormal()
    alpha = ot.DistFunc.rNormal(10)
    beta =  ot.DistFunc.rNormal(6*14)
    gamma =  ot.DistFunc.rNormal(20*14)

    # b1
    b1 = [20] * 10 + list(alpha)

    # b2
    b2 = [[0] *20] * 20
    for i in range(6):
        for j in range(6):
            b2[i][j] = -15.0

    # Tae into account beta
    k = 0
    for i in range(6):
        for j in range(14):
            b2[i][j + 6] = beta[k]
            k = k + 1

    # Take into account gamma
    k = 0
    for i in range(6, 20):
        for j in range(20):
            b2[i][j] = gamma[k]

    # b3
    b3 = [[[0]*20]*20]*20
    for i in range(5):
        for j in range(5):
            for k in range(5):
                b3[i][j][k] = -10.0

    # b4
    b4 = [[[[0]*20]*20]*20]*20
    for i in range(4):
        for j in range(4):
            for k in range(4):
                for l in range(4):
                    b4[i][j][k][l] = 5

    # X is a list, transform it into array
    X = ot.Point([x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20])
    w = (X - [0.5]*20)*2
    for k in [2,4,6]:
        w[k] = 2.0 * (1.1 * X[k] / (X[k] + 0.1) - 0.5)

    y = 0.0
    y = ot.dot(w, b1)

    # Morris function
    for i in range(19):
        for j in range(i + 1, 20):
            y +=  b2[i][j] * w[i] * w[j]
    for i in range(18):
        for j in range(i + 1, 19):
            for k in range(j + 1, 20):
                y += b3[i][j][k] * w[i] * w[j] * w[k]

    for i in range(17):
        for j in range(i + 1, 18):
            for k in range(j + 1, 20):
                for l in range(k + 1, 20):
                    y += b4[i][j][k][l] * w[i] * w[j] * w[k] * w[l]

    y = y + b0
    y_fake = 0.

    return y

"""


myStudy = otguibase.OTStudy('myStudy')

inputs = []
for i in range(20):
    inputs.append(otguibase.Input('x' + str(i + 1), ot.Uniform(0, 1)))
y = otguibase.Output('y')
y_fake = otguibase.Output('y_fake')
print(inputs)
model = otguibase.PythonPhysicalModel('MorrisModel', inputs, [y, y_fake], code)

myStudy.add(model)

# Morris ##
analysis = otguibase.MorrisAnalysis('aMorris', model)
analysis.setInterestVariables(['y'])
analysis.setLevel(4)
analysis.setTrajectoriesNumber(4)
analysis.setSeed(2)
myStudy.add(analysis)
print(analysis)

analysis.run()

print("error message=", analysis.getErrorMessage())
print("result=", analysis.getResult())
print("meanAbsEE =", analysis.getResult().getMeanAbsoluteElementaryEffects())
# script
script = myStudy.getPythonScript()

otguibase.OTStudy.Add(myStudy)
print(script)
exec(script)
