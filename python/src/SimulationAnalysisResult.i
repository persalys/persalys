// SWIG file

%{
#include "otgui/SimulationAnalysisResult.hxx"
%}

%include SimulationAnalysisResult_doc.i

%include otgui/SimulationAnalysisResult.hxx
namespace OTGUI {

%extend SimulationAnalysisResult { SimulationAnalysisResult(const SimulationAnalysisResult & other) { return new OTGUI::SimulationAnalysisResult(other); } 

std::string __repr__() {
  return "SimulationAnalysisResult";
}

} }
