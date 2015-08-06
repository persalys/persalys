#include <iostream>
// OT includes
#include <OT.hxx>
#include "PhysicalModel.hxx"

using namespace OT;
using namespace OTGUI;

int main(int argc, char **argv)
{
  PhysicalModel model("unamed");
  std::cout << model.dump() << std::endl;
  return EXIT_SUCCESS;
}

