// SWIG file

%{
#include "otgui/FunctionalChaosAnalysisResult.hxx"
%}

%include FunctionalChaosAnalysisResult_doc.i

%include otgui/FunctionalChaosAnalysisResult.hxx
namespace OTGUI {

%extend FunctionalChaosAnalysisResult { FunctionalChaosAnalysisResult(const FunctionalChaosAnalysisResult & other) { return new OTGUI::FunctionalChaosAnalysisResult(other); } 

std::string __repr__() {
  return "FunctionalChaosAnalysisResult";
}

} }
