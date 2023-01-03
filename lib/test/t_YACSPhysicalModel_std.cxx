//                                               -*- C++ -*-
/**
 *  @brief The test file for YACSPhysicalModel
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/PythonEnvironment.hxx"

#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include <iostream>

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  String pyscript =
"def _exec(x, y, p):\n"
"  w, z=3*x+4*y+p,x+y+p\n"
"  return w,z\n";

  try
  {
    PythonEnvironment env;
    YACSPhysicalModel myPhysicalModel("myPhysicalModel", InputCollection(), OutputCollection(), pyscript);
    Sample inputSample(3, 3);
    inputSample(0, 0) = 1;
    inputSample(1, 0) = 2;
    inputSample(2, 0) = 6;
    inputSample(0, 1) = 2;
    inputSample(1, 1) = 3;
    inputSample(2, 1) = 1;
    inputSample(0, 2) = 4;
    inputSample(1, 2) = 5;
    inputSample(2, 2) = 3;

    Sample evalSample(3, 2);
    evalSample(0, 0) = 15.;
    evalSample(0, 1) = 7.;
    evalSample(1, 0) = 23.;
    evalSample(1, 1) = 10.;
    evalSample(2, 0) = 25.;
    evalSample(2, 1) = 10.;

    Sample resultSample(myPhysicalModel.getFunction()(inputSample));
    std::cout << resultSample << std::endl;

    // Comparison
    assert_almost_equal(evalSample, resultSample, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
