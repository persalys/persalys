import persalys
import openturns as ot

myStudy = persalys.Study("myStudy")

truncated_input = persalys.Input("X0", 0, ot.TruncatedDistribution(ot.Normal(), -1, 1))
user_defined_input = persalys.Input("X1", 2, ot.UserDefined([[1], [2], [3]], [1, 1, 2]))
normal_input = persalys.Input("X2", 1, ot.Normal(1, 2))
output = persalys.Output("Y0")
inputs = [truncated_input, user_defined_input, normal_input]
outputs = [output]
formulas = ["X0+X1+X2"]
model = persalys.SymbolicPhysicalModel("model", inputs, outputs, formulas)
myStudy.add(model)

script = myStudy.getPythonScript()

namespace = {}
exec(script, namespace)

for i in range(len(inputs)):
    assert inputs[i].getDistribution() == namespace["inputs"][i].getDistribution()
