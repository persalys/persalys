#! /usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import openturns.testing
import persalys
import os

myStudy = persalys.Study('myStudy')

# data
filename = 'données.csv'
ot.RandomGenerator_SetSeed(0)
ot.Normal(3).getSample(10).exportToCSVFile(filename)
inColumns = [0, 2]

# Model 1
model = persalys.DataModel('myDataModel', filename, inColumns)
myStudy.add(model)
print(model)

# Model 2
model2 = persalys.SymbolicPhysicalModel('SM', [persalys.Input('A'), persalys.Input('B')], [persalys.Output('S')], ['A+B+2'])
myStudy.add(model2)
importedDOE = persalys.ImportedDesignOfExperiment('doeI', model2, filename, inColumns)
myStudy.add(importedDOE)

# script
script = myStudy.getPythonScript()
print(script)

# save
xmlFileName = 'file_with_données.xml'
myStudy.save(xmlFileName)

# open
s = persalys.Study.Open('file_with_données.xml')
print(s.getPythonScript())

os.remove(filename)
os.remove(xmlFileName)
