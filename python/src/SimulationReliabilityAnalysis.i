// SWIG file SimulationReliabilityAnalysis.i

%{
#include "otgui/SimulationReliabilityAnalysis.hxx"
%}

%include SimulationReliabilityAnalysis_doc.i

%include otgui/SimulationReliabilityAnalysis.hxx
namespace OTGUI {

%extend SimulationReliabilityAnalysis { SimulationReliabilityAnalysis(const SimulationReliabilityAnalysis & other) { return new OTGUI::SimulationReliabilityAnalysis(other); } 

} }
