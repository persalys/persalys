// SWIG file MonteCarloReliabilityAnalysis.i

%{
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
%}

%include MonteCarloReliabilityAnalysis_doc.i

%include otgui/MonteCarloReliabilityAnalysis.hxx
namespace OTGUI {

%extend MonteCarloReliabilityAnalysis { MonteCarloReliabilityAnalysis(const MonteCarloReliabilityAnalysis & other) { return new OTGUI::MonteCarloReliabilityAnalysis(other); } 

} }
