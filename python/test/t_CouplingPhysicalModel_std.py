#!/usr/bin/env python

from __future__ import print_function
import os
import math
import sys
import persalys
import openturns as ot
import openturns.testing as ott

### testcase A: parse output with tokens
with open('input_template.txt', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('external_program.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("Y0=%.17e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.17e\\n" % Y1)\n')


input_file = persalys.CouplingInputFile('input_template.txt')
input_file.setConfiguredPath('input.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'],
                        ['{:10.6f}', '{:10.6f}', '{:10.6f}'])
resource_file = persalys.CouplingResourceFile('external_program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt',
                             [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('A', [step])

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

# post-processing
step.setCode("def _exec2(Y0, Y1): \n    Y2 = Y0+3*Y1 \n    Y3 = Y2+3*Y1 \n    return Y2, Y3 \n")
model = persalys.CouplingPhysicalModel('A', [step])
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0, 27.0, 48.0])

# mis-spelled function name
step.setCode("def _exe-c2(Y0, Y1): \n    Y2 = Y0+3*Y1 \n    Y3 = Y2+3*Y1 \n    return Y2, Y3 \n")
model = persalys.CouplingPhysicalModel('A', [step])
f = model.getFunction()
try:
    f(x)
    print("FAIL")
except Exception:
    print("OK")

# mis-spelled variable names
step.setCode("def _exec2(Y-0, Y1): \n    Y2 = Y0+3*Y1 \n    Y3 = Y2+3*Y1 \n    return Y2, Y3 \n")
model = persalys.CouplingPhysicalModel('A', [step])
f = model.getFunction()
try:
    f(x)
    print("FAIL")
except Exception:
    print("OK")

try:
    output_file.setVariables(['Y-0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
    print("FAIL")
except TypeError:
    print("OK")


# cleanup
os.remove('input_template.txt')
os.remove('external_program.py')



### testcase D: parse output with table
with open('input_template.txt', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('external_program.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("%.17e\\n" % Y0)\n')
    f.write('    f.write("%.17e\\n" % Y1)\n')


input_file = persalys.CouplingInputFile('input_template.txt')
input_file.setConfiguredPath('input.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('external_program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['']*2, [0, 0], [0, -1], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt',
                             [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('D', [step])

# single evaluation
x = [1.0, 2.0, 3.0]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0])

# cleanup
os.remove('input_template.txt')
os.remove('external_program.py')

### testcase E: parse output and skip token
with open('input_template.txt', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('external_program.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    #Voluntarily adding an empty line w. token so we can skip it
    f.write('    f.write("Y0=")\n')
    f.write('    f.write("Y0=%.3e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.3e\\n" % Y1)\n')

input_file = persalys.CouplingInputFile('input_template.txt')
input_file.setConfiguredPath('input.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('external_program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [1, 0], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt',
                             [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('E', [step])

# single evaluation
x = [1.0, 2.0, 3.0]
model.setCleanupWorkDirectory(False)
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0])

# cleanup
os.remove('input_template.txt')
os.remove('external_program.py')


### testcase reusing temp dir
with open('input.txt.in', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('program.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("Y0=%.17e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.17e\\n" % Y1)\n')


input_file = persalys.CouplingInputFile('input.txt.in')
input_file.setConfiguredPath('input.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' program.py input.txt',
                             [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('reuse1', [step])
# leave work dir
model.setCleanupWorkDirectory(False)

# single evaluation
x = [1.0, 2.0, 3.0]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0])

# reuse previous work dir, with an empty command
step = persalys.CouplingStep('', [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('reuse2', [step])
f = model.getFunction()
y = f(x)
ott.assert_almost_equal(y, [6.0, 7.0])

# cleanup
os.remove('input.txt.in')
os.remove('program.py')

### testcase using file cache
with open('input.txt.in', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('program.py', 'w') as f:
    f.write('import sys\n')
    f.write('exec(open(sys.argv[1]).read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output.txt", "w") as f:\n')
    f.write('    f.write("Y0=%.17e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.17e\\n" % Y1)\n')


input_file = persalys.CouplingInputFile('input.txt.in')
input_file.setConfiguredPath('input.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' program.py input.txt',
                             [input_file], [resource_file], [output_file])
model = persalys.CouplingPhysicalModel('csvcache', [step])
model.setCacheFiles('in.csv', 'out.csv')

# single evaluation
x = [1.0, 2.0, 3.0]
f = persalys.PhysicalModel(model).getFunction()
for i in range(5):
    y = f(x)
print(y)
print('n calls=', f.getCallsNumber())

ott.assert_almost_equal(y, [6.0, 7.0])

# cleanup
os.remove('input.txt.in')
os.remove('program.py')
os.remove('in.csv')
os.remove('out.csv')

### testcase with several steps

# step1: Y0,Y1=f1(X0, X1, X2)
with open('input1.txt.in', 'w') as f:
    f.write('X0=@X0\n')
    f.write('X1=@X1\n')
    f.write('X2=@X2\n')
with open('program1.py', 'w') as f:
    f.write('exec(open("input1.txt").read())\n')
    f.write('Y0=X0+X1+X2\n')
    f.write('Y1=X0+X1*X2\n')
    f.write('with open("output1.txt", "w") as f:\n')
    f.write('    f.write("Y0=%.17e\\n" % Y0)\n')
    f.write('    f.write("Y1=%.17e\\n" % Y1)\n')
input_file = persalys.CouplingInputFile('input1.txt.in')
input_file.setConfiguredPath('input1.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('program1.py')
output_file = persalys.CouplingOutputFile('output1.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0], [0, 0])
step1 = persalys.CouplingStep(sys.executable + ' program1.py',
                              [input_file], [resource_file], [output_file])

# step2: Y2=f2(X2, X3, X4)
with open('input2.txt.in', 'w') as f:
    f.write('X2=@X2\n')
    f.write('X3=@X3\n')
    f.write('X4=@X4\n')
with open('program2.py', 'w') as f:
    f.write('exec(open("input2.txt").read())\n')
    f.write('Y2=27+X2+X3+X4\n')
    f.write('with open("output2.txt", "w") as f:\n')
    f.write('    f.write("Y2=%.17e\\n" % Y2)\n')
input_file = persalys.CouplingInputFile('input2.txt.in')
input_file.setConfiguredPath('input2.txt')
input_file.setVariables(['X2', 'X3', 'X4'], ['@X2', '@X3', '@X4'], ['', '', ''])
resource_file = persalys.CouplingResourceFile('program2.py')
output_file = persalys.CouplingOutputFile('output2.txt')
output_file.setVariables(['Y2'], ['Y2='], [0], [0], [0])
step2 = persalys.CouplingStep(sys.executable + ' program2.py',
                              [input_file], [resource_file], [output_file])

# step3: Z0=f3(Y1, Y2)
with open('input3.txt.in', 'w') as f:
    f.write('Y1=@Y1\n')
    f.write('Y2=@Y2\n')
with open('program3.py', 'w') as f:
    f.write('exec(open("input3.txt").read())\n')
    f.write('Z0=100+Y1+Y2\n')
    f.write('with open("output3.txt", "w") as f:\n')
    f.write('    f.write("Z0=%.17e\\n" % Z0)\n')
input_file = persalys.CouplingInputFile('input3.txt.in')
input_file.setConfiguredPath('input3.txt')
input_file.setVariables(['Y1', 'Y2'], ['@Y1', '@Y2'], ['', ''])
resource_file = persalys.CouplingResourceFile('program3.py')
output_file = persalys.CouplingOutputFile('output3.txt')
output_file.setVariables(['Z0'], ['Z0='], [0], [0], [0])
step3 = persalys.CouplingStep(sys.executable + ' program3.py',
                              [input_file], [resource_file], [output_file])

# step4: no inputs, no outputs
step4 = persalys.CouplingStep('echo 42', [], [], [])
step4.setIsShell(True)

model = persalys.CouplingPhysicalModel('multi', [step1, step2, step3, step4])
print('input variables=', model.getInputNames())
print('output variables=', model.getOutputNames())

# single evaluation
x = [1.0, 2.0, 3.0, 4.0, 5.0]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [6.0, 7.0, 39.0, 146.0])

# cleanup
os.remove('input1.txt.in')
os.remove('program1.py')
os.remove('input2.txt.in')
os.remove('program2.py')
os.remove('input3.txt.in')
os.remove('program3.py')

# script
myStudy = persalys.Study('myStudy')
myStudy.add(model)
script = myStudy.getPythonScript()
#print('script=', script)
exec(script)
