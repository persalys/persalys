//                                               -*- C++ -*-
/**
 *  @brief The test file of class Function for history mechanism
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <openturns/OT.hxx>
#include <openturns/OTtestcode.hxx>
#include "persalys/FileMemoizeEvaluation.hxx"

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

class FooEvaluation : public EvaluationImplementation
{
public:
  FooEvaluation() : EvaluationImplementation() {}

  FooEvaluation * clone() const override
  {
    return new FooEvaluation(*this);
  }

  UnsignedInteger getInputDimension() const override
  {
    return 2;
  }
  UnsignedInteger getOutputDimension() const override
  {
    return 3;
  }

  Point operator()(const Point & inP) const override
  {
    const Scalar x0 = inP[0];
    const Scalar x1 = inP[1];
    if (x1 < 0.0)
      throw InvalidArgumentException(HERE) << "x1<0 at " << inP.__str__();
    const Scalar y0 = x0 + x1;
    const Scalar y1 = x0 * x1;
    const Scalar y2 = y0 + y1;
    return {y0, y1, y2};
  }

  Sample operator()(const Sample & inS) const override
  {
    const UnsignedInteger size = inS.getSize();
    Sample result(size, getOutputDimension());
    Indices okIndices;
    Indices failedIndices;
    Description errors;
    for (UnsignedInteger i = 0; i < size; ++ i)
    {
      try
      {
        result[i] = operator()(inS[i]);
        okIndices.add(i);
      }
      catch (const Exception & exc)
      {
        errors.add(exc.what());
        failedIndices.add(i);
      }
    }
    if (failedIndices.getSize())
      throw BatchFailedException(HERE, failedIndices, errors, okIndices, result.select(okIndices)) << "operator(Sample) partial fail";
    return result;
  }
};


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    FileMemoizeEvaluation multi(new SymbolicEvaluation(Description({"x"}), Description({"y0", "y1", "y2", "y3", "y4"}), Description({"x", "x^2", "x^3", "x^4", "x^5"})), "input1.csv", "output1.csv");
    Function memoMulti(multi);
    // memoMulti(input);
    Indices indices = {3, 1};
    Function marginal(memoMulti.getMarginal(indices));
    fullprint << "memoized marginal=" << marginal << std::endl;
    remove("input1.csv");
    remove("output1.csv");
    
    FileMemoizeEvaluation g2(new SymbolicEvaluation(Description({"x"}), Description({"y"}), Description(1, "x^3")), "input2.csv", "output2.csv");
    Function f2(g2);
    fullprint << "default gradient=" << f2.getUseDefaultGradientImplementation() << std::endl;
    remove("input3.csv");
    remove("output3.csv");

    // test Batch exception
    const FileMemoizeEvaluation g3(new FooEvaluation, "input3.csv", "output3.csv");
    const Function f3(g3);
    Sample X(Normal(2).getSample(8) + Point({0.0, 10.0}));
    X.add(Normal(2).getSample(2) + Point({0.0, -10.0}));
    std::cout << X << std::endl;
    try
    {
      f3(X);
    }
    catch (const BatchFailedException & exc)
    {
      assert_equal(exc.getFailedIndices(), Indices({8, 9}));
      assert_equal(exc.getSucceededIndices(), Indices({8, 9}).complement((X.getSize())));

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
    remove("input3.csv");
    remove("output3.csv");
  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
