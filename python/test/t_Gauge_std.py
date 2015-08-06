#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import otguibase

myStudy = otguibase.Study("myStudy")

XMLfilename = "path_to_file.xml"

myPhysicalModel = YACSPhysicalModel("myPhysicalModel", XMLfilename)

