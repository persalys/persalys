//                                               -*- C++ -*-
/**
 *  @brief The test of validation Cogeneration
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "persalys/Persalys.hxx"

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

int main(int /*argc*/, char ** /*argv*/)
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    PERSALYS::Study myStudy("myStudy");

    Input Q("Q", 10200., Normal(10200, 100), "Primary energy");
    Input E("E", 3000., Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000., Normal(4000, 60), "Valued thermal energy");

    Output Ep("Ep", "Primary energy savings");
    SymbolicPhysicalModel symbolic("myPhysicalModel");

    symbolic.addInput(Q);
    symbolic.addInput(E);
    symbolic.addInput(C);

    symbolic.addOutput(Ep);
    symbolic.setFormula("Ep", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    PhysicalModel myPhysicalModel(symbolic);
    myStudy.add(myPhysicalModel);

    // First parametric analysis
    Collection<Point> values(3, Point(2));
    const Interval bounds(GridDesignOfExperiment::GetDefaultBounds(myPhysicalModel));
    for (UnsignedInteger i = 0; i < 3; ++i)
    {
      values[i][0] = bounds.getLowerBound()[i];
      values[i][1] = bounds.getUpperBound()[i];
    }
    GridDesignOfExperiment aDesign("aDesign", myPhysicalModel, values);
    myStudy.add(aDesign);
    aDesign.run();
    Sample resultSample1(aDesign.getResult().getDesignOfExperiment().getOutputSample());
    assert_almost_equal(resultSample1.computeMean()[0], 0.0595903);
    assert_almost_equal(resultSample1.computeStandardDeviation()[0], 0.0203342);

    // Second parametric analysis
    aDesign.getResult().getDesignOfExperiment().getInputSample().exportToCSVFile("tCogenSample.csv");
    Indices inputIndices(3);
    inputIndices.fill();
    ImportedDesignOfExperiment anotherdesign("anotherDesign", myPhysicalModel, "tCogenSample.csv", inputIndices);
    anotherdesign.run();
    Sample resultSample2(anotherdesign.getResult().getDesignOfExperiment().getOutputSample());

    // Reference
    assert_almost_equal(resultSample1, resultSample2, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
