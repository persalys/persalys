#!/usr/bin/env python

import gc

import persalys
import openturns as ot

ot.ResourceMap.SetAsUnsignedInteger("PythonPhysicalModel-DefaultProcessNumber", 2)


# A Study does not own its content: a Python script can create a study, add objects to it
# and drop it, while keeping references to the objects it contained. A study observes
# everything it contains in order to be kept in sync with them, so it must stop observing
# them when it is destroyed. Otherwise the next notification sent by one of them calls
# into the destroyed study, which crashes.


def buildStudy(name):
    X0 = persalys.Input("X0", 1.0, ot.Normal(), "")
    Y0 = persalys.Output("Y0", "")
    model = persalys.PythonPhysicalModel(
        "model", [X0], [Y0], "def _exec(X0):\n    Y0 = X0\n    return Y0"
    )

    study = persalys.Study(name)
    study.add(model)

    analysis = persalys.MonteCarloAnalysis("analysis", model)
    analysis.setMaximumCalls(20)
    study.add(analysis)

    return study, model, analysis


def testRefCount():
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
    PhysicalModel_0.setProcessNumber(1)
    Study_0.add(PhysicalModel_0)
    print(Study_0.getPythonScript())


def testObserverIsReleased():
    study, model, analysis = buildStudy("study")
    del study
    gc.collect()
    print("study dropped, model and analysis kept", flush=True)

    # notify the observers of the physical model
    model.setCode("def _exec(X0):\n    Y0 = 2 * X0\n    return Y0")
    model.setProcessNumber(2)
    print("physical model notified its observers", flush=True)

    # notify the observers of the analysis
    analysis.run()
    print("analysis notified its observers", flush=True)

    # same thing with a model shared by two studies: dropping one study must leave the
    # objects usable by whoever still holds them
    otherStudy, model, _ = buildStudy("otherStudy")
    del otherStudy
    gc.collect()
    model.setCode("def _exec(X0):\n    Y0 = 3 * X0\n    return Y0")
    print("model of a dropped study notified its observers", flush=True)


testRefCount()
testObserverIsReleased()
print("OK", flush=True)
