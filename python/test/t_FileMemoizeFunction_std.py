#! /usr/bin/env python

import openturns as ot
import persalys
import os

g = ot.SymbolicFunction(['x1', 'x2'], ['x1+x2', '3*(x1+x2)', 'x1*x2'])
f = persalys.FileMemoizeFunction(g, 'in_fm.csv', 'out_fm.csv')
for i in range(5):
    f([1, 2])
    f([10+i, 20+i])
print('f.in', f.getCacheInput())
print('f.out', f.getCacheOutput())
print('f.hits', f.getCacheHits())
print('f.calls', f.getCallsNumber())
g2 = ot.SymbolicFunction(['x1', 'x2'], ['?']*3)
f2 = persalys.FileMemoizeFunction(g2, 'in_fm.csv', 'out_fm.csv')
for i in range(5):
    f2([1, 2])
    f2([10+i, 20+i])
print('f2.in', f2.getCacheInput())
print('f2.out', f2.getCacheOutput())
print('f2.hits', f2.getCacheHits())
print('f2.calls', f2.getCallsNumber())

os.remove(f.getCacheInputFile())
os.remove(f.getCacheOutputFile())
