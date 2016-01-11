#include <iostream>
// OT includes
#include <OT.hxx>
#include "otgui/PhysicalModel.hxx"
#include "otgui/PythonPhysicalModel.hxx"

using namespace OT;
using namespace OTGUI;

int main(int argc, char **argv)
{
  PhysicalModel model("unamed");
  std::cout << model.dump() << std::endl;

  PythonPhysicalModel pythonModel("unamed");
  std::cout << pythonModel.dump() << std::endl;
  return EXIT_SUCCESS;
}

