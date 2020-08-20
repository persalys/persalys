#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import persalys
import os

# Model with error
X0 = persalys.Input('X0', 1, ot.Normal())
X1 = persalys.Input('X1', 2)
Y0 = persalys.Output('Y0')
model2 = persalys.PythonPhysicalModel('model2',  [X0, X1], [Y0], 'from math import pi\n\ndef _exec(X0, X1):\n    Y0 = 2*X1 - 1/X0\n    return Y0\n')
model2.setParallel(True)

design5 = persalys.FixedDesignOfExperiment('design5', model2)
design5.setOriginalInputSample([[0.9, 1.1], [1.8, 2.2], [0, 2.2]])
design5.setBlockSize(3)
design5.run()
assert design5.getResult().getDesignOfExperiment().getOutputSample().getSize() == 2, "whole block must no fail"
