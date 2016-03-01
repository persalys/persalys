//                                               -*- C++ -*-
/**
 *  @brief The test file for PhysicalModel
 *
 *  Copyright 2015-2016 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iostream>
// OT includes
#include <OT.hxx>
#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"
#include "otgui/OTStudy.hxx"
#include "Python.h"

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
  Py_Initialize();
  for (unsigned int i = 0; i < models.size(); ++ i)
  {
    PhysicalModel & model = models[i];
    model.addInput(Q);
    model.addInput(E);
    model.addInput(C);
    model.addOutput(Ep);
    study.add(model);
    std::cout << model.getFunction()(x) << std::endl;
  }
  Py_Finalize();
  std::cout << study.getPythonScript() << std::endl;
  return EXIT_SUCCESS;
}

