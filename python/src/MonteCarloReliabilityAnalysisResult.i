// SWIG file

%{
#include "otgui/MonteCarloReliabilityAnalysisResult.hxx"
%}

%include MonteCarloReliabilityAnalysisResult_doc.i

%include otgui/MonteCarloReliabilityAnalysisResult.hxx
namespace OTGUI {

%extend MonteCarloReliabilityAnalysisResult { MonteCarloReliabilityAnalysisResult(const MonteCarloReliabilityAnalysisResult & other) { return new OTGUI::MonteCarloReliabilityAnalysisResult(other); } 

std::string __repr__() {
  return "MonteCarloReliabilityAnalysisResult";
}

} }
