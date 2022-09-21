#!/usr/bin/env python

import openturns as ot
import persalys

# proba
dist_X0 = ot.Normal(0, 1)
X0 = persalys.Input("X0", 0, dist_X0, "")
dist_X1 = ot.Normal(0, 1)
X1 = persalys.Input("X1", 0, dist_X1, "")
X2 = persalys.Input("X2", 0, "")
X3 = persalys.Input("X3", 0, "")
Y0 = persalys.Output("Y0", "")
inputs = [X0, X1, X2, X3]
outputs = [Y0]
formulas = ["X0+X1+X2+X3"]
SymbolicModel_0 = persalys.SymbolicPhysicalModel(
    "SymbolicModel_0", inputs, outputs, formulas
)
values = [[0], [0], [-0.1, 0.1], [-0.1, 0.1]]
design_0 = persalys.GridDesignOfExperiment("design_0", SymbolicModel_0, values)
design_0.setBlockSize(1)
interestVariables = ["Y0"]
design_0.setInterestVariables(interestVariables)
design_0.run()
metaModel_0 = persalys.FunctionalChaosAnalysis("metaModel_0", design_0)
metaModel_0.setChaosDegree(1)
metaModel_0.setSparseChaos(False)
metaModel_0.setAnalyticalValidation(True)
metaModel_0.setTestSampleValidation(False)
metaModel_0.setKFoldValidation(False)
metaModel_0.run()
print(metaModel_0.getResult())

# deterministic/chaos
X0 = persalys.Input("X0", 0, "")
X1 = persalys.Input("X1", 0, "")
X2 = persalys.Input("X2", 0, "")
X3 = persalys.Input("X3", 0, "")
Y0 = persalys.Output("Y0", "")
inputs = [X0, X1, X2, X3]
outputs = [Y0]
formulas = ["X0+X1+X2+X3"]
SymbolicModel_0 = persalys.SymbolicPhysicalModel(
    "SymbolicModel_0", inputs, outputs, formulas
)
values = [[0], [0], [-0.1, 0.1], [-0.1, 0.1]]
design_0 = persalys.GridDesignOfExperiment("design_0", SymbolicModel_0, values)
design_0.setBlockSize(1)
interestVariables = ["Y0"]
design_0.setInterestVariables(interestVariables)
design_0.run()
metaModel_0 = persalys.FunctionalChaosAnalysis("metaModel_0", design_0)
metaModel_0.setChaosDegree(1)
metaModel_0.setSparseChaos(False)
metaModel_0.setAnalyticalValidation(True)
metaModel_0.setTestSampleValidation(False)
metaModel_0.setKFoldValidation(False)
metaModel_0.run()
print(metaModel_0.getResult())

# deterministic/kriging
X0 = persalys.Input("X0", 0, "")
X1 = persalys.Input("X1", 0, "")
X2 = persalys.Input("X2", 0, "")
X3 = persalys.Input("X3", 0, "")
Y0 = persalys.Output("Y0", "")
inputs = [X0, X1, X2, X3]
outputs = [Y0]
formulas = ["X0+X1+X2+X3"]
SymbolicModel_0 = persalys.SymbolicPhysicalModel(
    "SymbolicModel_0", inputs, outputs, formulas
)
values = [[0], [0], [-0.1, 0.1], [-0.1, 0.1]]
design_0 = persalys.GridDesignOfExperiment("design_0", SymbolicModel_0, values)
design_0.setBlockSize(1)
interestVariables = ["Y0"]
design_0.setInterestVariables(interestVariables)
design_0.run()
metaModel_0 = persalys.KrigingAnalysis("metaModel_0", design_0)
metaModel_0.setBasis(ot.ConstantBasisFactory(2).build())
metaModel_0.setCovarianceModel(ot.SquaredExponential(2))
metaModel_0.setAnalyticalValidation(True)
metaModel_0.setTestSampleValidation(False)
metaModel_0.setKFoldValidation(False)
metaModel_0.run()
print(metaModel_0.getResult())
