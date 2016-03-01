//                                               -*- C++ -*-
/**
 *  @brief The test of validation Cogeneration
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
#include "OT.hxx"
#include "OTtestcode.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/AnalyticalPhysicalModel.hxx"
#include "otgui/DesignOfExperiment.hxx"


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

    Input Q("Q", 0., "Primary energy", Normal(10200, 100));
    Input E("E", 0., "Produced electric energy", Normal(3000, 15));
    Input C("C", 0., "Valued thermal energy", Normal(4000, 60));

    Output Ep("Ep", 0., "Primary energy savings", "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    AnalyticalPhysicalModel myPhysicalModel("myPhysicalModel");

    myPhysicalModel.addInput(Q);
    myPhysicalModel.addInput(E);
    myPhysicalModel.addInput(C);

    myPhysicalModel.addOutput(Ep);

    myStudy.add(myPhysicalModel);

    // Parametric analysis
    DesignOfExperiment aDesign("aDesign", myPhysicalModel);
    myStudy.add(aDesign);
    aDesign.evaluate();
    NumericalSample resultSample(aDesign.getResult().getOutputSample());

    // Reference

    Description inputVariables(3);
    inputVariables[0] = "Q";
    inputVariables[1] = "E";
    inputVariables[2] = "C";
    Description outputVariables(1);
    outputVariables[0] = "Ep";
    Description formula(1);
    formula[0] = (OSS() << "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    NumericalMathFunction f(inputVariables, outputVariables, formula);


    ComposedDistribution::DistributionCollection distributions;
    distributions.add(Normal(10200,100));
    distributions.add(Normal(3000,15));
    distributions.add(Normal(4000,60));

    NumericalPoint scale(3);
    NumericalPoint transvec(3);
    NumericalPoint levels(3, 0.);

    for (int i=0; i<3; ++i)
    {
      double a = distributions[i].computeQuantile(0.05)[0];
      double b = distributions[i].computeQuantile(0.95)[0];
      scale[i] = b-a;
      transvec[i] = a;
    }

    Box box(levels);
    NumericalSample inputSample(box.generate());
    inputSample*=scale;
    inputSample+=transvec;

    NumericalSample outputSample=f(inputSample);

    // Comparaison
    assert_almost_equal(outputSample, resultSample, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
