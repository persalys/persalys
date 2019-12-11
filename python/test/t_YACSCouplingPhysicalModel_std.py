#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import persalys
import os
import math
import sys


### testcase A: parse output with tokens
with open('input_template0y.txt', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('external_program0y.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("Y0=%.17e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.17e\\n" % Y1)\n')


input_file1 = persalys.CouplingInputFile('input.txt')
input_file1.setTemplatePath('input_template0y.txt')
input_file1.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
input_file2 = persalys.CouplingInputFile('external_program0y.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program0y.py input.txt', [input_file1, input_file2], [output_file])
model = persalys.YACSCouplingPhysicalModel('A', [step])

# single evaluation
x = [1.0, 2.0, 3.0]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0])

# sampling test
ot_function = model.getFunction()
Y = ot.CompositeRandomVector(ot_function, ot.RandomVector(ot.Normal(3)))
y = Y.getSample(50)
mean, stddev = y.computeMean(), y.computeStandardDeviationPerComponent()
print(mean, stddev)
ott.assert_almost_equal(mean, [0, 0], 0.3, 0.3)
ott.assert_almost_equal(stddev, [math.sqrt(3.0), 1.415], 0.3, 0.3)

# cleanup
os.remove('input_template0y.txt')
os.remove('external_program0y.py')

# script
myStudy = persalys.Study('myStudy')
myStudy.add(model)
script = myStudy.getPythonScript()
#print('script=', script)
exec(script)
