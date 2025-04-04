//                                               -*- C++ -*-
/**
 *  @brief The test file for YACSPhysicalModel
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

  String pyscript2 =
    "def _exec(x, y, p):\n"
    "  w, z=3*x+4*y+p+1,x+y+p\n"
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

    Sample evalSample2(3, 2);
    evalSample2(0, 0) = 16.;
    evalSample2(0, 1) = 7.;
    evalSample2(1, 0) = 24.;
    evalSample2(1, 1) = 10.;
    evalSample2(2, 0) = 26.;
    evalSample2(2, 1) = 10.;

    Sample resultSample(myPhysicalModel.getFunction()(inputSample));
    std::cout << resultSample << std::endl;
    myPhysicalModel.setCode(pyscript2);
    Sample resultSample2(myPhysicalModel.getFunction()(inputSample));
    std::cout << resultSample2 << std::endl;


    // Comparison
    assert_almost_equal(evalSample, resultSample, 1e-16);
    assert_almost_equal(evalSample2, resultSample2, 1e-16);

    String failingScript =
      "from math import sqrt\n"
      "def _exec(X0):\n"
      "  Y0 = 1/sqrt(X0)\n"
      "  return Y0";

    YACSEvaluation eval(failingScript);
    Sample X(3, 1);
    X(0, 0) = -1.;
    X(1, 0) = 0.;
    X(2, 0) = 1.;

    Sample sample2(1, 1);
    sample2(0, 0) = 1;

    const Function f3(eval);
    try
    {
      resultSample = f3(X);
    }
    catch (const BatchFailedException & exc)
    {
      resultSample = exc.getOutputSample();
    }
    assert_almost_equal(sample2, resultSample, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
