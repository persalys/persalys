// SWIG file

%{
#include "otgui/SimulationAnalysis.hxx"
%}


%include otgui/SimulationAnalysis.hxx
namespace OTGUI {

%extend SimulationAnalysis { SimulationAnalysis(const SimulationAnalysis & other) { return new OTGUI::SimulationAnalysis(other); } 

std::string __repr__() {
  return "SimulationAnalysis";
}

} }
