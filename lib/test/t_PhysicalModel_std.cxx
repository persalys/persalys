//                                               -*- C++ -*-
/**
 *  @brief The test file for PhysicalModel
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iostream>
// OT includes
#include "openturns/OT.hxx"
#include "persalys/Persalys.hxx"

using namespace OT;
using namespace PERSALYS;

int main(int /*argc*/, char ** /*argv*/)
{
  PythonEnvironment pyEnv;

  PERSALYS::Study study("study1");
  Input Q("Q", 0., Normal(10200, 100), "Primary energy");
  Input E("E", 0., Normal(3000, 15), "Produced electric energy");
  Input C("C", 0., Normal(4000, 60), "Valued thermal energy");
  Output Ep("Ep", "Primary energy savings");
  InputCollection inputCollection(3);
  inputCollection[0] = Q;
  inputCollection[1] = E;
  inputCollection[2] = C;
  OutputCollection outputCollection(1);
  outputCollection[0] = Ep;

  Point x(3);
  x[0] = 10200;
  x[1] = 3000;
  x[2] = 4000;

  Description formula(1, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
  SymbolicPhysicalModel analyticalModel("analyticalModel1", inputCollection, outputCollection, formula);
  study.add(analyticalModel);
  std::cout << analyticalModel.getFunction()(x) << std::endl;

  String code = "def _exec(Q, E, C):\n    Ep = 1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))\n    return Ep";
  PythonPhysicalModel pythonModel("pythonModel1", inputCollection, outputCollection, code);
  study.add(pythonModel);
  std::cout << pythonModel.getFunction()(x) << std::endl;

  std::cout << study.getPythonScript() << std::endl;
  return EXIT_SUCCESS;
}

