import os
import platform
import persalys
# import openturns as ot
import openturns.testing as ott

# testcase using ansys connector
key_platform = platform.system()
dict_exe = {
    "Linux": ["dummyAnsys", "./dummyAnsys"],
    "Windows": ["dummyAnsys.exe", "dummyAnsys.exe"],
}

pathHere = os.path.dirname(os.path.abspath(__file__))
pathResources = os.path.join(pathHere, "ansysConnector")
modelFile = os.path.join(pathResources, "BEAM.wbpj")
ansysExec = dict_exe[key_platform][0]
templateFile = "input.txt.in"

step = persalys.CouplingStep()
model = persalys.CouplingPhysicalModel("A", [step])

parser = persalys.AnsysParser(modelFile)
parser.populateCouplingStep(model, 0, templateFile)

# Required for testing because of dummyAnsys
step = model.getSteps()[0]
resourceFiles = step.getResourceFiles()
resourceFiles.add(persalys.CouplingResourceFile(ansysExec))
step.setResourceFiles(resourceFiles)
command = step.getCommand()
step.setCommand(dict_exe[key_platform][1] + command)

model.setSteps([step])
x = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0]
f = model.getFunction()
y = f(x)
print(y)
ott.assert_almost_equal(y, [0.10662])

os.remove(templateFile)
