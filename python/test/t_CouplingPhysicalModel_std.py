#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import openturns.coupling_tools as otct
import persalys
import os
import platform
import tempfile
import shutil
import math
import sys
import hashlib
import struct

class CouplingResourceFile(object):
    def __init__(self, path):
        if not os.path.isabs(path):
            raise ValueError('path to input file must be absolute')
        self.path_ = path
    def getPath(self):
        return self.path_
class CouplingInputFile(object):
    def __init__(self, path, template_path=''):
        if os.path.isabs(path):
            raise ValueError('path to input file cannot be absolute')
        # When template_path==None this file is not configured, just a copied data file
        # the template_path can be absolute
        self.template_path_ = template_path
        self.path_ = path
        self.tokens_ = []
        self.varnames_ = []
    def setVariables(self, varnames, tokens):
        self.tokens_ = tokens
        if len(varnames) != len(tokens):
            raise ValueError('tokens must have the same size as varnames')
        self.varnames_ = varnames
    def getVariableNames(self):
        return self.varnames_
    def setTemplatePath(self, template_path):
        self.template_path_ = template_path
    def getTemplatePath(self):
        return self.template_path_
    def getPath(self):
        return self.path_
    def getTokens(self):
        return self.tokens_
class CouplingOutputFile(object):
    def __init__(self, path):
        if os.path.isabs(path):
            raise ValueError('path to output file cannot be absolute')
        self.path_ = path
        self.tokens_ = []
        self.skip_lines_ = []
        self.skip_cols_ = []
        self.varnames_ = []

    def setVariables(self, varnames, tokens, skip_lines=None, skip_cols=None):
        self.tokens_ = tokens
        if len(varnames) != len(tokens):
            raise ValueError('tokens must have the same size as varnames')
        self.varnames_ = varnames
        # empty str => None token
        for k in range(len(tokens)):
            if not tokens[k]:
                self.tokens_[k] = None
        if skip_lines is None:
            self.skip_lines_ = [0] * len(tokens)
        else:
            if len(skip_lines) != len(self.tokens_):
                raise ValueError('skip_lines must have the same size as tokens')
            self.skip_lines_ = skip_lines
        if skip_cols is None:
            self.skip_cols_ = [0] * len(tokens)
        else:
            if len(skip_cols) != len(self.tokens_):
                raise ValueError('skip_cols must have the same size as tokens')
            self.skip_cols_ = skip_cols
    def getVariableNames(self):
        return self.varnames_
    def getPath(self):
        return self.path_
    def getTokens(self):
        return self.tokens_
    def getSkipLines(self):
        return self.skip_lines_
    def getSkipColumns(self):
        return self.skip_cols_
class CouplingStep(object):
    def __init__(self, command, input_files, resource_files, output_files):
        self.command_ = command
        self.is_shell_ = False
        self.input_files_ = input_files
        self.resource_files_ = resource_files
        self.output_files_ = output_files
    def getIsShell(self):
        return self.is_shell_
    def getInputFiles(self):
        return self.input_files_
    def getResourceFiles(self):
        return self.resource_files_
    def getOutputFiles(self):
        return self.output_files_
    def getCommand(self):
        return self.command_
class CouplingPhysicalModel(object):
    def __init__(self, name, steps):
        if len(steps) == 0:
            raise ValueError('at least one step is required')
        self.steps_ = steps
        # cache input variables
        self.input_variables_ = self.getInputVariables()
        self.cleanup_work_dir_ = True
        self.name_ = name

    def getInputVariables(self):
        # variables of all input files (no duplicates), exclude outputs
        inputs = []
        outputs = self.getOutputVariables()
        for step in self.steps_:
            for input_file in step.getInputFiles():
                for varname in input_file.getVariableNames():
                    if (not varname in inputs) and (not varname in outputs):
                        inputs.append(varname)
        return inputs

    def getInputDimension(self):
        return len(self.getInputVariables())

    def getOutputVariables(self):
        # variables of all output files (no duplicates)
        outputs = []
        for step in self.steps_:
            for output_file in step.getOutputFiles():
                for varname in output_file.getVariableNames():
                    if not varname in outputs:
                        outputs.append(varname)
        return outputs

    def getOutputDimension(self):
        return len(self.getOutputVariables())

    def getCleanupWorkDirectory(self):
        return self.cleanup_work_dir_

    def getFunction(self):
        return ot.PythonFunction(self.getInputDimension(), self.getOutputDimension(), self)

    def getInputNames(self):
        return ot.Description(self.getInputVariables())
    def getOutputNames(self):
        return ot.Description(self.getOutputVariables())
    def __call__(self, X):
        if len(X) != len(self.input_variables_):
            raise ValueError('wrong input dimension')

        # initialize known current values
        all_vars = dict(zip(self.input_variables_, X))

        # everything happens in a temporary work dir
        #workdir = tempfile.mkdtemp()
        checksum = hashlib.sha1()
        [checksum.update(hex(struct.unpack('<Q', struct.pack('<d', x))[0]).encode()) for x in X]
        workdir = os.path.join(tempfile.gettempdir(), 'persalys_' + checksum.hexdigest())
        if not os.path.exists(workdir):
            os.makedirs(workdir)

        # for each command
        for step in self.steps_:

            # 1. write input files
            for resource_file in step.getResourceFiles():
                if not input_file.getPath():
                    continue
                # just a file/dir that needs to be copied
                if os.path.isfile(input_file.getPath()):
                    shutil.copy(input_file.getPath(), os.path.join(workdir, input_file.getPath()))
                elif os.path.isdir(input_file.getPath()):
                    shutil.copytree(input_file.getPath(), os.path.join(workdir, input_file.getPath()))
                else:
                    raise ValueError('cannot handle file:', input_file.getPath())
            for resource_file in step.getResourceFiles():
                # generate input file from template
                input_values = [all_vars[varname] for varname in input_file.getVariableNames()]
                otct.replace(input_file.getTemplatePath(), os.path.join(workdir, input_file.getPath()), input_file.getTokens(), input_values)

            # 2. run command
            if len(step.getCommand()) > 0:
                otct.execute(step.getCommand(), workdir=workdir, is_shell=step.getIsShell())

            # 3. read values
            for output_file in step.getOutputFiles():
                if not output_file.getPath():
                    continue
                outfile = os.path.join(workdir, output_file.getPath())
                for varname, token, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):
                    all_vars[varname] = otct.get_value(outfile, token=token, skip_line=skip_line, skip_col=skip_col)

        # cleanup work dir
        if self.getCleanupWorkDirectory():
            shutil.rmtree(workdir)

        # return the output values
        output_values = [all_vars[varname] for varname in self.getOutputVariables()]
        return output_values

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


input_file = persalys.CouplingInputFile('input.txt')
input_file.setTemplatePath('input_template.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
resource_file = persalys.CouplingResourceFile('external_program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt', [input_file], [resource_file], [output_file])
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


input_file = persalys.CouplingInputFile('input.txt')
input_file.setTemplatePath('input_template.txt')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
resource_file = persalys.CouplingResourceFile('external_program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['']*2, [0, -1], [0, 0])
step = persalys.CouplingStep(sys.executable + ' external_program.py input.txt', [input_file], [resource_file], [output_file])
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


input_file = persalys.CouplingInputFile('input.txt')
input_file.setTemplatePath('input.txt.in')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
resource_file = persalys.CouplingResourceFile('program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' program.py input.txt', [input_file], [resource_file], [output_file])
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


input_file = persalys.CouplingInputFile('input.txt')
input_file.setTemplatePath('input.txt.in')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
resource_file = persalys.CouplingResourceFile('program.py')
output_file = persalys.CouplingOutputFile('output.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0])
step = persalys.CouplingStep(sys.executable + ' program.py input.txt', [input_file], [resource_file], [output_file])
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
input_file = persalys.CouplingInputFile('input1.txt')
input_file.setTemplatePath('input1.txt.in')
input_file.setVariables(['X0', 'X1', 'X2'], ['@X0', '@X1', '@X2'])
resource_file = persalys.CouplingResourceFile('program1.py')
output_file = persalys.CouplingOutputFile('output1.txt')
output_file.setVariables(['Y0', 'Y1'], ['Y0=', 'Y1='], [0, 0], [0, 0])
step1 = persalys.CouplingStep(sys.executable + ' program1.py', [input_file], [resource_file], [output_file])

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
input_file = persalys.CouplingInputFile('input2.txt')
input_file.setTemplatePath('input2.txt.in')
input_file.setVariables(['X2', 'X3', 'X4'], ['@X2', '@X3', '@X4'])
resource_file = persalys.CouplingResourceFile('program2.py')
output_file = persalys.CouplingOutputFile('output2.txt')
output_file.setVariables(['Y2'], ['Y2='], [0], [0])
step2 = persalys.CouplingStep(sys.executable + ' program2.py', [input_file], [resource_file], [output_file])

# step3: Z0=f3(Y1, Y2)
with open('input3.txt.in', 'w') as f:
    f.write('Y1=@Y1\n')
    f.write('Y2=@Y2\n')
with open('program3.py', 'w') as f:
    f.write('exec(open("input3.txt").read())\n')
    f.write('Z0=100+Y1+Y2\n')
    f.write('with open("output3.txt", "w") as f:\n')
    f.write('    f.write("Z0=%.17e\\n" % Z0)\n')
input_file = persalys.CouplingInputFile('input3.txt')
input_file.setTemplatePath('input3.txt.in')
input_file.setVariables(['Y1', 'Y2'], ['@Y1', '@Y2'])
resource_file = persalys.CouplingResourceFile('program3.py')
output_file = persalys.CouplingOutputFile('output3.txt')
output_file.setVariables(['Z0'], ['Z0='], [0], [0])
step3 = persalys.CouplingStep(sys.executable + ' program3.py', [input_file], [resource_file], [output_file])

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

