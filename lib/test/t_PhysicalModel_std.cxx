#include <iostream>
// OT includes
#include <OT.hxx>
#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/OTStudy.hxx"

using namespace OT;
using namespace OTGUI;

int main(int argc, char **argv)
{
  OTStudy study("study1");
  Input Q("Q", 0., "Primary energy", Normal(10200, 100));
  Input E("E", 0., "Produced electric energy", Normal(3000, 15));
  Input C("C", 0., "Valued thermal energy", Normal(4000, 60));
  Output Ep("Ep", 0., "Primary energy savings", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");

  NumericalPoint x(3);
  x[0] = 10200;
  x[1] = 3000;
  x[2] = 4000;

  AnalyticalPhysicalModel analyticalModel("analyticalModel1");

  PythonPhysicalModel pythonModel("pythonModel1");
  pythonModel.setCode("def _exec(X):\n    Q = X[0]\n    E = X[1]\n    C = X[2]\n    Ep = 1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))\n    return [Ep]");

  std::vector<PhysicalModel> models;
  models.push_back(analyticalModel);
  models.push_back(pythonModel);
  for (unsigned int i = 0; i < models.size(); ++ i)
  {
    PhysicalModel & model = models[i];
    model.addInput(Q);
    model.addInput(E);
    model.addInput(C);
    model.addOutput(Ep);
    study.addPhysicalModel(model);
    std::cout << model.getFunction()(x) << std::endl;
  }
  
  std::cout << study.dump() << std::endl;
  return EXIT_SUCCESS;
}

