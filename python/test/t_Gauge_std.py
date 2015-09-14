#! /usr/bin/env python

from __future__ import print_function
import sys
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy('myStudy')

XMLfilename = 'gauge.xml'
if len(sys.argv) > 1:
    XMLfilename = sys.argv[1]
myPhysicalModel = otguibase.YACSPhysicalModel('myPhysicalModel', XMLfilename)
myPhysicalModel.loadDataWithYACS()
myStudy.addPhysicalModel(myPhysicalModel)

print(myPhysicalModel.getInputs())
print(myPhysicalModel.getOutputs())
