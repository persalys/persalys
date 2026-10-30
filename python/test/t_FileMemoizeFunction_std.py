#! /usr/bin/env python

import openturns as ot
import openturns.testing
import persalys
import os

g = ot.SymbolicFunction(["x1", "x2"], ["x1+x2", "3*(x1+x2)", "x1*x2"])
f = persalys.FileMemoizeFunction(g, "in_fm.csv", "out_fm.csv")
for i in range(5):
    f([1, 2])
    f([10 + i, 20 + i])
print("f.in", f.getCacheInput())
print("f.out", f.getCacheOutput())
print("f.hits", f.getCacheHits())
print("f.calls", f.getCallsNumber())
print("f.evals", f.getEvaluationCallsNumber())
g2 = ot.SymbolicFunction(["x1", "x2"], ["?"] * 3)
f2 = persalys.FileMemoizeFunction(g2, "in_fm.csv", "out_fm.csv")
for i in range(5):
    f2([1, 2])
    f2([10 + i, 20 + i])
print("f2.in", f2.getCacheInput())
print("f2.out", f2.getCacheOutput())
print("f2.hits", f2.getCacheHits())
print("f2.calls", f2.getCallsNumber())
print("f2.evals", f2.getEvaluationCallsNumber())

os.remove(f.getCacheInputFile())
os.remove(f.getCacheOutputFile())

# Memoized marginal (FileMemoizeEvaluation)
g_multi = ot.SymbolicFunction(["x"], ["x", "x^2", "x^3", "x^4", "x^5"])
memo_multi = persalys.FileMemoizeEvaluation(g_multi.getEvaluation(), "input1.csv", "output1.csv")
memo_multi_func = ot.Function(memo_multi)
marginal = memo_multi_func.getMarginal([3, 1])
print("memoized marginal=", marginal)
print("marginal at x=2:", marginal([2.0]))
openturns.testing.assert_almost_equal(marginal([2.0]), [16.0, 4.0], 1e-12)

os.remove("input1.csv")
os.remove("output1.csv")
