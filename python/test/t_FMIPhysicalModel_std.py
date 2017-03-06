#!/usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott
import otguibase
import os
import platform

myStudy = otguibase.OTStudy('myStudy')

key_platform = (platform.system(), platform.architecture()[0])
# Call to either 'platform.system' or 'platform.architecture' *after*
# importing pyfmi causes a segfault.
dict_platform = {('Linux', '64bit'):'linux64',
                ('Windows', '32bit'):'win32'}

path_here = os.path.dirname(os.path.abspath(__file__))
try:
    directory_platform = dict_platform[key_platform]
except KeyError:
    raise RuntimeError('Not available')

path_fmu = os.path.join(path_here, 'fmu', directory_platform, 'deviation.fmu')

E = otguibase.Input('E', 1.0, ot.Normal(), 'E description')
F = otguibase.Input('F', 1.0)
I = otguibase.Input('I', 1.0)
L = otguibase.Input('L', 1.0)
y = otguibase.Output('y', 'deviation')

inputs = [E, F, I, L]
outputs = [y]
model = otguibase.FMIPhysicalModel('myPhysicalModel', inputs, outputs, path_fmu)
myStudy.add(model)

#print(model.getCode())

E_d = ot.Beta(0.93, 3.2, 28000000.0, 48000000.0)
F_d = ot.LogNormalMuSigma(30000.0, 9000.0, 15000.0).getDistribution()
L_d = ot.Uniform(250.0, 260.0)
I_d = ot.Beta(2.5, 4.0, 310.0, 450.0)
distribution = ot.ComposedDistribution([E_d, F_d, I_d, L_d])
x = distribution.getMean()


f = model.getFunction()
y = f(x)
ott.assert_almost_equal(y, [12.3369])

## script
script = myStudy.getPythonScript()
print('script=', script)
exec(script)


# compare with pyfmi directly
if 0:
    import pyfmi
    import otfmi

    #print(path_fmu)
    model_fmu = model_fmu = otfmi.FMUFunction(
                    path_fmu, inputs_fmu=['E', 'F', 'I', 'L'], outputs_fmu='y')


    ml = pyfmi.load_fmu(path_fmu)
    vars = ml.get_model_variables()
    for var in vars.values():
        print(var.name, var.value_reference)
        #print(ml.get_real(var.value_reference))
