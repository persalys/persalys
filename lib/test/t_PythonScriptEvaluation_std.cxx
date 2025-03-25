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
    PythonScriptEvaluation eval(Description(1, "X0"), Description(1, "Y0"),  code);
    eval.setParallel(true);

    // test Batch exception
    const Function g3(eval);
    const MemoizeFunction f3(g3);
    Sample X(Normal(1).getSample(10));
    X.add(X.select(Indices({0, 1, 3, 8, 9})));
    std::cout << X << std::endl;
    try
    {
      f3(X);
    }
    catch (const BatchFailedException & exc)
    {
      std::cout << "i_fail=" << exc.getFailedIndices() << std::endl;
      std::cout << "X_fail=" << X.select(exc.getFailedIndices()) << std::endl;
      for (UnsignedInteger i = 0; i < exc.getFailedIndices().getSize(); ++ i)
        std::cout << "i_fail=" << exc.getFailedIndices()[i] << " error=" << exc.getErrorDescription()[i] << std::endl;
      std::cout << "i_ok=" << exc.getSucceededIndices() << std::endl;
      std::cout << "X_ok=" << X.select(exc.getSucceededIndices()) << std::endl;
      std::cout << "Y_ok=" << exc.getOutputSample() << std::endl;
      std::cout << "f(X_ok)=" << f3(X.select(exc.getSucceededIndices())) << std::endl;
      std::cout << "what=" << exc.what() << std::endl;
    }
  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
