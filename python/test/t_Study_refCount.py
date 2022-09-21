#!/usr/bin/env python


import persalys


def test():

    Study_0 = persalys.Study("Study_0")
    persalys.Study.Add(Study_0)
    X0 = persalys.Input("X0", 0, "")
    Y0 = persalys.Output("Y0", "")
    Y0.setValue(0)
    inputCollection = []
    inputCollection.append(X0)
    outputCollection = []
    outputCollection.append(Y0)
    code = "def _exec(X0):\n    Y0 = X0\n    return Y0"
    PhysicalModel_0 = persalys.PythonPhysicalModel(
        "PhysicalModel_0", inputCollection, outputCollection, code
    )
    Study_0.add(PhysicalModel_0)
    print(Study_0.getPythonScript())


test()
