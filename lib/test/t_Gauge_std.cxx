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

  try
  {
    OTStudy myStudy("myStudy");

    YACSPhysicalModel myPhysicalModel("myPhysicalModel", "example.xml");
    myPhysicalModel.loadDataWithYACS();
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
