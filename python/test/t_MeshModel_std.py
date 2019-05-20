#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function
import openturns as ot
import persalys

# mesh model
meshModel0 = persalys.GridMeshModel()

print(meshModel0)

# mesh model
meshModel1 = persalys.GridMeshModel([persalys.Variable('x', 'aDescription')], ot.Interval(0., 12.), ot.Indices(1, 20))

print(meshModel1)

filename = 'mesh.csv'
mesh = ot.IntervalMesher(ot.Indices(1, 20)).build(ot.Interval(0., 12.))
mesh.getVertices().exportToCSVFile(filename)

# mesh model
meshModel2 = persalys.ImportedMeshModel()

print(meshModel2)

# mesh model
meshModel3 = persalys.ImportedMeshModel([persalys.Variable('z', 'aDescription')], 'mesh.csv', ot.Indices(1, 0))

print(meshModel3)
