// SWIG file

%{
#include "otgui/SimulationCalculus.hxx"
%}


%include otgui/SimulationCalculus.hxx
namespace OTGUI {

%extend SimulationCalculus { SimulationCalculus(const SimulationCalculus & other) { return new OTGUI::SimulationCalculus(other); } 

std::string __repr__() {
  return "SimulationCalculus";
}

} }
