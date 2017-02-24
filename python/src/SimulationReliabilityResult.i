// SWIG file

%{
#include "otgui/SimulationReliabilityResult.hxx"
%}

%include SimulationReliabilityResult_doc.i

%include otgui/SimulationReliabilityResult.hxx
namespace OTGUI {

%extend SimulationReliabilityResult { SimulationReliabilityResult(const SimulationReliabilityResult & other) { return new OTGUI::SimulationReliabilityResult(other); } 

} }
