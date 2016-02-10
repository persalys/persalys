//                                               -*- C++ -*-
/**
 *  @brief The test of validation Gauge
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
