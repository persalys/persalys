//                                               -*- C++ -*-

#include "OT.hxx"
#include "OTtestcode.hxx"
#include "OTStudy.hxx"
#include "ParametricCalculus.hxx"


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
    PhysicalModel myPhysicalModel("myPhysicalModel");

    myPhysicalModel.addInput(Q);
    myPhysicalModel.addInput(E);
    myPhysicalModel.addInput(C);

    myPhysicalModel.addOutput(Ep);

    myStudy.addPhysicalModel(myPhysicalModel);

    // Parametric analysis
    ParametricCalculus calculus("myParametricCalculus", myPhysicalModel);
    calculus.run();
    NumericalSample resultSample(calculus.getResultSample());

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
