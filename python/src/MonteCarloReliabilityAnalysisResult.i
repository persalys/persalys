// SWIG file

%{
#include "otgui/MonteCarloReliabilityAnalysisResult.hxx"
%}


%include otgui/MonteCarloReliabilityAnalysisResult.hxx
namespace OTGUI {

%extend MonteCarloReliabilityAnalysisResult { MonteCarloReliabilityAnalysisResult(const MonteCarloReliabilityAnalysisResult & other) { return new OTGUI::MonteCarloReliabilityAnalysisResult(other); } 

std::string __repr__() {
  return "MonteCarloReliabilityAnalysisResult";
}

} }
