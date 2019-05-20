#! /usr/bin/env python

from __future__ import print_function
import sys
import openturns as ot
import persalys

myStudy = persalys.Study('myStudy')

XMLfilename = 'gauge.xml'
if len(sys.argv) > 1:
    XMLfilename = sys.argv[1]
myPhysicalModel = persalys.YACSPhysicalModel('myPhysicalModel', XMLfilename)
myStudy.add(myPhysicalModel)

print(myPhysicalModel.getInputs())
print(myPhysicalModel.getOutputs())
