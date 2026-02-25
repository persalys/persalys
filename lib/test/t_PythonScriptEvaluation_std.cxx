//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for history mechanism
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "persalys/Persalys.hxx"
#include "persalys/PythonScriptEvaluation.hxx"

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    PythonEnvironment pyEnv;
    String code = "from math import sqrt\ndef _exec(X0):\n    Y0 = sqrt(X0)\n    return Y0";
    for (UnsignedInteger parallel = 0; parallel < 2; ++ parallel)
    {
      PythonScriptEvaluation eval(Description({"X0"}), Description({"Y0"}), code);
      eval.setParallel(parallel);

      // test Batch exception
      const Function f3(eval);
      Sample X(Normal(10.0, 1.0).getSample(8));
      X.add(Normal(-10.0, 1.0).getSample(2));
      std::cout << X << std::endl;
      try
      {
        f3(X);
      }
      catch (const BatchFailedException & exc)
      {
        assert_equal(exc.getFailedIndices(), Indices({8, 9}));
        assert_equal(exc.getSucceededIndices(), Indices({8, 9}).complement((X.getSize())));

        const String msg(exc.what());
        std::cerr << "what=" << msg << std::endl;
        if (msg.substr(0, 28) != "Batch evaluation 2/10 failed")
          throw InvalidArgumentException(HERE) << "wrong exception";
      }
    }
  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
