#!/usr/bin/env python

from __future__ import print_function
import os
import math
import sys
import persalys
import openturns as ot
import openturns.testing as ott

with open('input_logistic.txt.in', 'w') as f:
    f.write('y0=@y0@\n')
    f.write('a=@a@\n')
    f.write('b=@b@\n')

with open('program_logistic.py', 'w') as f:
    f.write('import math as m\n')
    f.write('def logisticSolution(t, y0, a, b):\n')
    f.write('    t0 = 1790.0\n')
    f.write('    y = a * y0 / (b * y0 + (a - b * y0) * m.exp(-a * (t - t0)))\n')
    f.write('    return y\n\n')
    f.write('exec(open("input_logistic.txt").read())\n')
    f.write('Y = [logisticSolution(t, y0, a, b) for t in range(1790, 2011)]\n')
    f.write('with open("Y.txt", "w") as f:\n')
    f.write('    for y in Y:\n')
    f.write('        f.write("%.17g\\n" % y)\n')

with open('post_logistic.py', 'w') as f:
    f.write('import math as m\n')
    f.write('with open("Y.txt") as f:\n')
    f.write('    Y = [float(line) for line in f.readlines()]\n')
    f.write('Y_min = min(Y)\n')
    f.write('Y_max = max(Y)\n')
    f.write('Y_mean = sum(Y) / len(Y)\n')
    f.write('Y_last = Y[-1]\n')
    f.write('try:\n    idx = next(i for i,y in enumerate(Y) if y > 100e6)\n    Y_mean_t = sum(Y[:idx]) / idx\nexcept StopIteration:\n    Y_mean_t = 0.0\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("Y_min=%.17g\\n" % Y_min)\n')
    f.write('    f.write("Y_max=%.17g\\n" % Y_max)\n')
    f.write('    f.write("Y_mean=%.17g\\n" % Y_mean)\n')
    f.write('    f.write("Y_last=%.17g\\n" % Y_last)\n')
    f.write('    f.write("Y_mean_t=%.17g\\n" % Y_mean_t)\n')


input_file1 = persalys.CouplingInputFile('input_logistic.txt.in')
input_file1.setConfiguredPath('input_logistic.txt')
input_file1.setVariables(['y0', 'a', 'b'], ['@y0@', '@a@', '@b@'],
                        ['{:10.6f}', '{:10.6f}', '{:10.6f}'])
resource_file1 = persalys.CouplingResourceFile('program_logistic.py')
step1 = persalys.CouplingStep(sys.executable + ' program_logistic.py', [input_file1], [resource_file1], [])

resource_file2 = persalys.CouplingResourceFile('post_logistic.py')
output_file2 = persalys.CouplingOutputFile('output.txt')
output_file2.setVariables(['Y_min', 'Y_max', 'Y_mean', 'Y_last', 'Y_mean_t'], ['Y_min=', 'Y_max=', 'Y_mean=', 'Y_last=', 'Y_mean_t='], [0]*5, [0]*5, [0]*5)
step2 = persalys.CouplingStep(sys.executable + ' post_logistic.py', [], [resource_file2], [output_file2])

model = persalys.CouplingPhysicalModel('logistic', [step1, step2])
print(model.getInputNames())
print(model.getOutputNames())
assert model.getInputNames() == ['y0', 'a', 'b']
assert model.getOutputNames() == ['Y_min', 'Y_max', 'Y_mean', 'Y_last', 'Y_mean_t']

# single evaluation
x = [3.9e6, 0.03134, 1.5887e-10]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [3.9e+06, 3.8498e+09, 5.64037e+08, 3.8498e+09, 2.94863e+07])

# cleanup
os.remove('input_logistic.txt.in')
os.remove('program_logistic.py')
os.remove('post_logistic.py')

# script
myStudy = persalys.Study('myStudy')
myStudy.add(model)
script = myStudy.getPythonScript()
#print('script=', script)
exec(script)
