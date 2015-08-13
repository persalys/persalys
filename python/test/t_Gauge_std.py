#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.OTStudy("myStudy")

XMLfilename = "example.xml"
myPhysicalModel = otguibase.YACSPhysicalModel("myPhysicalModel", XMLfilename)
myPhysicalModel.loadDataWithYACS();
myStudy.addPhysicalModel(myPhysicalModel);

print(myPhysicalModel.getInputs())