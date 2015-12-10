//                                               -*- C++ -*-

#include "OT.hxx"
#include "OTtestcode.hxx"
#include "otgui/OTStudy.hxx"
#include "otgui/YACSPhysicalModel.hxx"


using namespace OT;
using namespace OT::Test;
using namespace OTGUI;

int main(int argc, char *argv[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);
  char *fileName = 0;
  if (argc > 1) {
    fileName = argv[1];
  }
  else {
    std::cerr << "Schema file (path to example.xml) not provided" << std::endl;
    return -1;
  }
  try
  {
    OTStudy myStudy("myStudy");

    YACSPhysicalModel myPhysicalModel("myPhysicalModel", fileName);
    myStudy.addPhysicalModel(myPhysicalModel);

    // Comparaison
//     assert_almost_equal(outputSample, resultSample, 1e-16);
  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
