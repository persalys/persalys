#!/usr/bin/env python
# coding: utf-8

from __future__ import print_function

import otguibase

def test():

    OTStudy_0 = otguibase.OTStudy('OTStudy_0')
    otguibase.OTStudy.Add(OTStudy_0)
    X0 = otguibase.Input('X0', 0, '')
    Y0 = otguibase.Output('Y0', '')
    Y0.setValue(0)
    inputCollection = []
    inputCollection.append(X0)
    outputCollection = []
    outputCollection.append(Y0)
    code = 'def _exec(X0):\n    Y0 = X0\n    return Y0'
    PhysicalModel_0 = otguibase.PythonPhysicalModel(
        'PhysicalModel_0', inputCollection, outputCollection, code)
    OTStudy_0.add(PhysicalModel_0)
    print(OTStudy_0.getPythonScript())

test()
