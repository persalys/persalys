#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing
import sys
import persalys

XMLfilename = 'example.xml'
if len(sys.argv) > 1:
    XMLfilename = sys.argv[1]

myStudy = persalys.Study('myStudy')

myPhysicalModel = persalys.YACSPhysicalModel('myPhysicalModel', XMLfilename)

myStudy.addPhysicalModel(myPhysicalModel)

inputSample = ot.Sample(3, 3)
inputSample[0] = [1, 2, 4]
inputSample[1] = [2, 3, 5]
inputSample[2] = [6, 1, 3]

evalSample = [[15, 7], [23, 10], [25, 10]]

resultSample = myPhysicalModel.getFunction()(inputSample)
print(resultSample)

# Comparison
openturns.testing.assert_almost_equal(evalSample, resultSample, 1e-16)
