//                                               -*- C++ -*-

#include "otgui/YACSPhysicalModel.hxx"

#include <OT.hxx>
#include "OTtestcode.hxx"
#include <iostream>

using namespace OT;
using namespace OT::Test;
using namespace OTGUI;

int main(int argc, char *argv[], char* env[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  char *fileName = 0;
  if (argc > 1) {
    fileName = argv[1];
  }
  else {
    std::cerr << "Schema file not provided" << std::endl;
    return -1;
  }
  try
  {
    YACSPhysicalModel myPhysicalModel("myPhysicalModel", fileName);
    NumericalSample inputSample(3, 3);
    inputSample[0][0] = 1;
    inputSample[1][0] = 2;
    inputSample[2][0] = 6;
    inputSample[0][1] = 2;
    inputSample[1][1] = 3;
    inputSample[2][1] = 1;
    inputSample[0][2] = 4;
    inputSample[1][2] = 5;
    inputSample[2][2] = 3;

    NumericalSample evalSample(3, 2);
    evalSample[0][0] = 15.;
    evalSample[0][1] = 7.;
    evalSample[1][0] = 23.;
    evalSample[1][1] = 10.;
    evalSample[2][0] = 25.;
    evalSample[2][1] = 10.;

    NumericalSample resultSample(myPhysicalModel.getFunction()(inputSample));
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
