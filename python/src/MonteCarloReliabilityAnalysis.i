// SWIG file

%{
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
%}



%include otgui/MonteCarloReliabilityAnalysis.hxx
namespace OTGUI {

%extend MonteCarloReliabilityAnalysis { MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other) { return new OTGUI::MonteCarloReliabilityAnalysis(other); } 

std::string __repr__() {
  return "MonteCarloReliabilityAnalysis";
}

} }
