//                                               -*- C++ -*-
/**
 *  @brief The test of validation Cogeneration
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"
#include "otgui/OTGui.hxx"

using namespace OT;
using namespace OT::Test;
using namespace OTGUI;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    OTStudy myStudy("myStudy");

    Input Q("Q", 10200., Normal(10200, 100), "Primary energy");
    Input E("E", 3000., Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000., Normal(4000, 60), "Valued thermal energy");

    Output Ep("Ep", "Primary energy savings");
    SymbolicPhysicalModel myPhysicalModel("myPhysicalModel");

    myPhysicalModel.addInput(Q);
    myPhysicalModel.addInput(E);
    myPhysicalModel.addInput(C);

    myPhysicalModel.addOutput(Ep);
    myPhysicalModel.setFormula("Ep", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");

    myStudy.add(myPhysicalModel);

    // First parametric analysis
    GridDesignOfExperiment aDesign("aDesign", myPhysicalModel);
    Indices levels(3, 2);
    aDesign.setLevels(levels);
    myStudy.add(aDesign);
    aDesign.run();
    Sample resultSample1(aDesign.getOutputSample());

    // Second parametric analysis
    aDesign.getInputSample().exportToCSVFile("sample.csv");
    Indices inputIndices(3);
    inputIndices.fill();
    ImportedDesignOfExperiment anotherdesign("anotherDesign", myPhysicalModel, "sample.csv", inputIndices);
    anotherdesign.run();
    Sample resultSample2(anotherdesign.getOutputSample());

    // Reference

    Description inputVariables(3);
    inputVariables[0] = "Q";
    inputVariables[1] = "E";
    inputVariables[2] = "C";
    Description formula(1);
    formula[0] = (OSS() << "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    SymbolicFunction f(inputVariables, formula);


    ComposedDistribution::DistributionCollection distributions;
    distributions.add(Normal(10200,100));
    distributions.add(Normal(3000,15));
    distributions.add(Normal(4000,60));

    Point scale(3);
    Point transvec(3);
    Point otLevels(3, 0.);

    for (int i=0; i<3; ++i)
    {
      double a = distributions[i].computeQuantile(0.05)[0];
      double b = distributions[i].computeQuantile(0.95)[0];
      scale[i] = b-a;
      transvec[i] = a;
    }

    Box box(otLevels);
    Sample inputSample(box.generate());
    inputSample*=scale;
    inputSample+=transvec;

    Sample outputSample=f(inputSample);

    // Comparaison
    assert_almost_equal(outputSample, resultSample1, 1e-16);
    assert_almost_equal(outputSample, resultSample2, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
